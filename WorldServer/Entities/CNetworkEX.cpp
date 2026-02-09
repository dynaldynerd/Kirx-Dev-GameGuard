#include "pch.h"

#include "CNetworkEX.h"

#include <cstdlib>
#include <cstring>

#include "FireGuard.h"
#include "CBillingManager.h"
#include "CCashDBWorkManager.h"
#include "CLogTypeDBTaskManager.h"
#include "CMainThread.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "CRaceBossMsgController.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "cancel_raceboss_msg_result_zoct.h"
#include "character_disconnect_result_wrac.h"
#include "connection_status_result_zoct.h"
#include "server_notify_inform_zone.h"

CNetWorking::CNetWorking()
{
  WSAData wsaData{};

  m_dwUseProcessNum = 0;
  if (WSAStartup(2u, &wsaData))
  {
    exit(0);
  }
  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion))
  {
    WSACleanup();
    exit(0);
  }

  for (int i = 0; i < 4; ++i)
  {
    m_pProcess[i] = &m_Process[i];
  }

  m_bUseFG = true;
  memset_0(m_szServerName, 0, sizeof(m_szServerName));
}

CNetWorking::~CNetWorking()
{
  Release();
}

_socket *CNetWorking::GetSocket(unsigned int dwProID, unsigned int dwSocketIndex)
{
  if (dwProID < this->m_dwUseProcessNum)
    return this->m_Process[dwProID].m_NetSocket.GetSocket(dwSocketIndex);
  return nullptr;
}

void CNetWorking::CloseSocket(unsigned int dwProID, unsigned int dwSocketIndex, bool bSlowClose)
{
  if (!bSlowClose
      || !this->m_Process[dwProID].m_FC.PushNode(dwSocketIndex,
        this->m_Process[dwProID].m_NetSocket.GetSocket(dwSocketIndex)->m_dwSerial))
  {
    this->m_Process[dwProID].PushCloseNode(dwSocketIndex);
  }
}

bool CNetWorking::SetNetSystem(
  unsigned int dwUseProcessNum,
  _NET_TYPE_PARAM *pType,
  char *szSystemName,
  char *pszLogPath)
{
  char szFileName[152]{};
  char returnedString[28]{};
  char filename[280]{};

  this->m_dwUseProcessNum = dwUseProcessNum;
  wsprintfA(this->m_szSystemName, "%s", szSystemName);
  if (pszLogPath)
  {
    strcpy_s(this->m_szLogPath, sizeof(this->m_szLogPath), pszLogPath);
  }
  else
  {
    strcpy_s(this->m_szLogPath, sizeof(this->m_szLogPath), ".\\NetLog");
  }

  CreateDirectoryA(this->m_szLogPath, nullptr);

  const unsigned int korLocalTime = GetKorLocalTime();
  wsprintfA(szFileName, "%s\\%s_Sys%d.log", this->m_szLogPath, this->m_szSystemName, korLocalTime);
  this->m_LogFile.SetWriteLogFile(szFileName, 1, 1, 1, 1);

  const unsigned int fgLogTime = GetKorLocalTime();
  wsprintfA(szFileName, "%s\\%s_CcrFgSys%d.log", this->m_szLogPath, this->m_szSystemName, fgLogTime);
  g_FgLogFile.SetWriteLogFile(szFileName, 1, 1, 1, 1);

  std::memset(returnedString, 0, 16);
  GetPrivateProfileStringA("FireGuard Use", "Use", "TRUE", returnedString, 0x10u, ".\\fireguard\\fgrs.ini");
  this->m_bUseFG = strcmp_0(returnedString, "FALSE") != 0;

  GetPrivateProfileStringA("System", "WorldName", "X", this->m_szServerName, 0x21u, "..\\WorldInfo\\WorldInfo.ini");
  if (this->m_szServerName[0] == 'X')
  {
    g_FgLogFile.Write("FG Error : WorldName Read Failed In \"WorldInfo.ini\" File");
    return false;
  }

  for (int nIndex = 0; nIndex < static_cast<int>(this->m_dwUseProcessNum); ++nIndex)
  {
    if (!this->m_Process[nIndex].SetProcess(nIndex, &pType[nIndex], this, this->m_bUseFG))
    {
      return false;
    }

    if (!nIndex)
    {
      std::memset(filename, 0, 260);
      GetModuleFileNameA(nullptr, filename, 0x104u);
      char *slash = strrchr(filename, '\\');
      if (slash)
      {
        *(slash + 1) = 0;
      }
      strcat_s(filename, 0x104u, "fireguard\\");
      const int pathLen = strlen_0(filename);
      if (AddEnvVariable("path", filename, pathLen) <= 0)
      {
        this->m_LogFile.Write("SetNetSystem(%d) CCRFG SERVER : AddEnvVariable() Fail", nIndex);
        return false;
      }

      if (this->m_bUseFG)
      {
        g_pfnCallBack.pfunc = CcrFgCallback;
        const unsigned int nameLen = strlen_0(this->m_szServerName);
        const int result = _CcrFG_rs_Initialize(
          reinterpret_cast<int (__fastcall *)(int, void *, void *, int, void *)>(g_pfnCallBack.pfunc),
          reinterpret_cast<unsigned __int8 *>(this->m_szServerName),
          nameLen);
        if (result < 1)
        {
          const unsigned int lastError = _CcrFG_rs_GetLastError();
          this->m_LogFile.Write(
            "SetNetSystem(%d) CCRFG SERVER : _CcrFG_rs_Initialize() Fail, nRet(%#x), _CcrFG_rs_GetLastError(%#x)",
            nIndex,
            result,
            lastError);
          return false;
        }
      }

      g_FGSendData.pNetwork = this;
      g_FGSendData.pTargetProc = &this->m_Process[nIndex];
    }
  }

  return true;
}

void CNetWorking::Release()
{
  for (unsigned int index = 0; index < this->m_dwUseProcessNum; ++index)
  {
    this->m_Process[index].Release();
    if (this->m_bUseFG)
    {
      _CcrFG_rs_Uninitialize();
    }
  }

  WSACleanup();
}

int CNetWorking::Connect(unsigned int dwProID, unsigned int dwSocketIndex, in_addr dwIP, unsigned __int16 wPort)
{
  sockaddr_in addr{};
  addr.sin_family = 2;
  addr.sin_port = htons(wPort);
  addr.sin_addr.S_un = dwIP.S_un;

  if (this->m_Process[dwProID].m_Type.m_bAnSyncConnect)
  {
    return this->m_Process[dwProID].PushAnsyncConnect(dwSocketIndex, &addr);
  }

  return this->m_Process[dwProID].m_NetSocket.Connect(dwSocketIndex, &addr);
}

void CNetWorking::OnLoop()
{
  for (unsigned int index = 0; index < this->m_dwUseProcessNum; ++index)
  {
    this->m_Process[index].OnLoop();
  }
  UserLoop();
}

void CNetWorking::OnLoop_Receipt()
{
  for (unsigned int index = 0; index < this->m_dwUseProcessNum; ++index)
  {
    this->m_Process[index].OnLoop_Receipt();
  }
}

void CNetWorking::ProcessLogFile(unsigned int dwProID, bool bRecv, bool bSend, bool bSystem)
{
  this->m_Process[dwProID].LogFileOperSetting(bRecv, bSend, bSystem);
}

unsigned int CNetWorking::GetCheckRecvTime(unsigned int dwProID, unsigned int dwSocketIndex)
{
  _socket *socket = GetSocket(dwProID, dwSocketIndex);
  if (socket)
  {
    return socket->m_dwLastRecvTime;
  }
  return 0xFFFFFFFFu;
}

void CNetWorking::UserLoop()
{
  // this is not a stub
}

bool CNetWorking::DataAnalysis(
  unsigned int dwProID,
  unsigned int dwClientIndex,
  _MSG_HEADER *pMsgHeader,
  char *pMsg)
{
  (void)dwProID;
  (void)dwClientIndex;
  (void)pMsgHeader;
  (void)pMsg;
  // this is not a stub
  return true;
}

void CNetWorking::AcceptClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial)
{
  (void)dwProID;
  (void)dwIndex;
  (void)dwSerial;
  // this is not a stub
}

void CNetWorking::CloseClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial)
{
  (void)dwProID;
  (void)dwIndex;
  (void)dwSerial;
  // this is not a stub
}

bool CNetWorking::ExpulsionSocket(unsigned int dwProID, unsigned int dwIndex, unsigned __int8 byReason, void *pvInfo)
{
  (void)byReason;
  (void)pvInfo;
  CloseSocket(dwProID, dwIndex, false);
  return true;
}

void CNetWorking::AnsyncConnectComplete(unsigned int dwProID, unsigned int dwIndex, int nResult)
{
  (void)dwProID;
  (void)dwIndex;
  (void)nResult;
  // this is not a stub
}

void CNetworkEX::SetPassablePacket(unsigned int dwProID, unsigned __int8 byHeader1, unsigned __int8 byHeader2)
{
  if (dwProID >= 4)
  {
    return;
  }
  if (m_pProcess[dwProID] == nullptr)
  {
    return;
  }
  m_pProcess[dwProID]->m_bPassablePacket[byHeader1][byHeader2] = true;
}

void CNetworkEX::Close(unsigned int dwProID, unsigned int dwSocketIndex, bool bSlowClose, const char *pszLog)
{
  if (pszLog)
  {
    m_LogFile.Write(pszLog);
  }
  CloseSocket(dwProID, dwSocketIndex, bSlowClose);
}

void CNetworkEX::UserLoop()
{
  // this is not a stub
}

void CNetworkEX::AcceptClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial)
{
  if (dwProID)
  {
    switch (dwProID)
    {
      case 1u:
        WriteServerStartHistory("Account Server Connectting Success >>");
        g_Main.AccountServerLogin();
        break;
      case 3u:
      {
        WriteServerStartHistory("Billing Agent Connectting Success >>");
        CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
        billing->Start();
        break;
      }
      case 2u:
        WriteServerStartHistory("Web Agent Connectting Success >>");
        break;
      default:
        break;
    }
  }
  else
  {
    if (g_Main.m_bServerClosing)
    {
      Close(0, dwIndex, false, nullptr);
    }
    else
    {
      CUserDB *user = &g_UserDB[dwIndex];
      if (user->m_bActive)
      {
        char buffer[136]{};
        sprintf(buffer, "Close Acceptclient : %s", user->m_szAccountID);
        Close(0, dwIndex, false, buffer);
      }
      else
      {
        _socket *socket = GetSocket(0, dwIndex);
        user->SetWorldCLID(dwSerial, reinterpret_cast<unsigned int *>(&socket->m_Addr.sin_addr.S_un.S_addr));
      }
    }
    ++CUserDB::s_nLoginNum;
  }

  if (dwProID && dwProID < 4)
  {
    m_byStatus[dwProID] = 1;
  }
}

void CNetworkEX::CloseClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial)
{
  (void)dwSerial;

  if (dwProID)
  {
    switch (dwProID)
    {
      case 1u:
        WriteServerStartHistory("Account Server Disconnectting >>");
        g_Main.gm_ServerClose();
        g_Main.m_bWorldOpen = false;
        g_Main.m_bWorldService = false;
        break;
      case 3u:
        WriteServerStartHistory("Billing Agent Disconnectting >>");
        break;
      case 2u:
        WriteServerStartHistory("Web Agent Disconnectting >>");
        if (dwIndex == g_Main.m_byWebAgentServerNetInx && g_Main.m_bConnectedWebAgentServer)
        {
          g_Main.m_bConnectedWebAgentServer = false;
          g_Main.m_byWebAgentServerNetInx = 0;
        }
        if (dwIndex == g_Main.m_byControllServerNetInx && g_Main.m_bConnectedControllServer)
        {
          g_Main.m_bConnectedControllServer = false;
          g_Main.m_byControllServerNetInx = 0;
        }
        break;
      default:
        break;
    }
  }
  else
  {
    CPlayer *player = &g_Player[dwIndex];
    player->NetClose(false);
    CUserDB *user = &g_UserDB[dwIndex];
    user->SendMsgAccount_UILockRefresh_Update();
    user->Exit_Account_Request();
    --CUserDB::s_nLoginNum;
  }

  if (dwProID && dwProID < 4)
  {
    m_byStatus[dwProID] = 0;
  }
}

void CNetworkEX::AnsyncConnectComplete(unsigned int dwProID, unsigned int dwIndex, int nResult)
{
  (void)dwIndex;

  if (dwProID == 1)
  {
    if (nResult)
    {
      __trace("Account Server Connectting Fail : %d", nResult);
    }
    else
    {
      __trace("Account Server Connectting Success");
    }
  }
}

bool CNetworkEX::DataAnalysis(
  unsigned int dwProID,
  unsigned int dwClientIndex,
  _MSG_HEADER *pMsgHeader,
  char *pMsg)
{
  switch (dwProID)
  {
    case 0u:
      return ClientLineAnalysis(dwClientIndex, pMsgHeader, pMsg);
    case 1u:
      return AccountLineAnalysis(dwClientIndex, pMsgHeader, pMsg);
    case 3u:
      return BillingLineAnalysis(static_cast<int>(dwClientIndex), pMsgHeader, pMsg);
    case 2u:
      return WebAgentLineAnalysis(static_cast<int>(dwClientIndex), pMsgHeader, pMsg);
    default:
      break;
  }
  return false;
}

bool CNetworkEX::ClientLineAnalysis(unsigned int n, _MSG_HEADER *pMsgHeader, char *pMsg)
{
  (void)n;
  (void)pMsgHeader;
  (void)pMsg;
  // TODO: Implement once IDA decompile is available (stub per request).
  return false;
}

bool CNetworkEX::AccountLineAnalysis(unsigned int n, _MSG_HEADER *pMsgHeader, char *pMsg)
{
  const unsigned __int8 type0 = pMsgHeader->m_byType[0];
  if (type0 == 1)
  {
    const int subType = static_cast<int>(pMsgHeader->m_byType[1]) - 2;
    switch (subType)
    {
      case 0:
        return OpenWorldSuccessResult(n, pMsg);
      case 4:
        return true;
      case 5:
        return ForceCloseCommand(n, reinterpret_cast<_CLID *>(pMsg));
      case 6:
        return TransAccountInform(n, pMsg);
      case 9:
        return EnterWorldResult(n, reinterpret_cast<_CLID *>(pMsg));
      case 14:
        return UILockInitResult(n, pMsg);
      case 16:
        return UILockUpdateResult(n, pMsg);
      case 18:
        return UILockRefreshResult(n, pMsg);
      case 19:
        return CheckIsBlockIPResult(n, pMsg);
      case 20:
        return OpenWorldFailureResult(n, pMsg);
      case 21:
        return ConEventTotalSalesCheck(static_cast<int>(n), pMsg);
      case 22:
        return DisconnectGuildWarCharacterRequest(static_cast<int>(n), pMsg);
      case 24:
        return ManageClientLimitRunRequest(pMsg);
      case 25:
        return ManageClientForceExitRequest();
      case 30:
        return CashDBInfoRecvResult(static_cast<int>(n), pMsg);
      default:
        return false;
    }
  }
  if (type0 == 50)
  {
    const unsigned __int8 type1 = pMsgHeader->m_byType[1];
    switch (type1)
    {
      case 1u:
        return WorldServiceInform(n, reinterpret_cast<bool *>(pMsg));
      case 3u:
        return WorldExitInform(n, pMsg);
      case 5u:
        return WorldMsgInform(n, pMsg);
      default:
        return type1 == 7 && ChatLockCommand(n, reinterpret_cast<_CLID *>(pMsg));
    }
  }
  return false;
}

bool CNetworkEX::BillingLineAnalysis(int n, _MSG_HEADER *pMsgHeader, char *pMsg)
{
  if (pMsgHeader->m_byType[0] != 29)
  {
    return true;
  }
  switch (pMsgHeader->m_byType[1])
  {
    case 7u:
      return BillingCloseRequest(n, pMsg);
    case 8u:
      return BillingRemaintimePersonal(n, pMsg);
    case 9u:
      return BillingRemaintimePCBang(n, pMsg);
    case 0xAu:
      return BillingChangeType(n, pMsg);
    case 0xBu:
      return BillingExpirePersonal(n, pMsg);
    case 0xCu:
      return BillingExpirePCBang(n, pMsg);
    case 0xDu:
      return BillingExpireIPOverflow(n, pMsg);
    case 0xEu:
      return BillingDestroyModule(n, pMsg);
    case 0xFu:
      return TaiwanBillingUserCertify(n, pMsg);
    case 0x11u:
      return ChinaBillingChangePrimium(n, pMsg);
    default:
      break;
  }
  return pMsgHeader->m_byType[1] != 90 || ZoneAliveCheckRequest(n, pMsg);
}

bool CNetworkEX::WebAgentLineAnalysis(int n, _MSG_HEADER *pMsgHeader, char *pMsg)
{
  if (pMsgHeader->m_byType[0] == 51)
  {
    const unsigned __int8 type1 = pMsgHeader->m_byType[1];
    if (!type1)
    {
      return LogInWebAgentServer(static_cast<unsigned int>(n), pMsg);
    }
    if (type1 == 12)
    {
      return SendRaceBossMsgFromWebRequest(n, pMsg);
    }
  }
  if (pMsgHeader->m_byType[0] != 54)
  {
    return true;
  }

  const unsigned __int8 type1 = pMsgHeader->m_byType[1];
  if (!type1)
  {
    return LogInControllServer(static_cast<unsigned int>(n), pMsg);
  }
  if (type1 == 3)
  {
    return CancelRaceBossSMSMsg(n, pMsg);
  }
  return type1 != 5 || ConnectionStatusRequest(n);
}

bool CNetworkEX::OpenWorldSuccessResult(unsigned int n, char *pMsg)
{
  (void)n;
  if (g_Main.m_byWorldType == static_cast<unsigned __int8>(pMsg[50]))
  {
    CMainThread::pc_OpenWorldSuccessResult(&g_Main, static_cast<unsigned __int8>(pMsg[0]), pMsg + 1, pMsg + 33);
  }
  else
  {
    MyMessageBox(
      "OpenWorld(Account -> Zone)",
      "!!Server type is wrong!!(AC:%d)(ZO:%d)",
      static_cast<unsigned __int8>(pMsg[50]),
      g_Main.m_byWorldType);
    WriteServerStartHistory(
      "ServerType is Wrong ==> AccountServer(%d) != ZoneServer(%d)",
      static_cast<unsigned __int8>(pMsg[50]),
      g_Main.m_byWorldType);
  }
  return true;
}

bool CNetworkEX::OpenWorldFailureResult(unsigned int n, char *pMsg)
{
  (void)n;
  CMainThread::pc_OpenWorldFailureResult(&g_Main, pMsg);
  return true;
}

bool CNetworkEX::ForceCloseCommand(unsigned int n, _CLID *pMsg)
{
  (void)n;
  _CLID *idWorld = pMsg;
  if (idWorld->wIndex > 0x9E4u)
  {
    return false;
  }
  const unsigned __int8 kickType = static_cast<unsigned __int8>(idWorld[1].wIndex >> 8);
  CMainThread::pc_ForceCloseCommand(
    &g_Main,
    idWorld,
    idWorld[1].wIndex,
    kickType,
    idWorld[1].dwSerial);
  return true;
}

bool CNetworkEX::TransAccountInform(unsigned int n, char *pMsg)
{
  (void)n;
  char *msg = pMsg;
  CMainThread::pc_TransIPKeyInform(
    &g_Main,
    reinterpret_cast<unsigned int *>(msg)[7],
    msg + 32,
    static_cast<unsigned __int8>(msg[45]),
    static_cast<unsigned __int8>(msg[46]),
    reinterpret_cast<unsigned int *>(msg) + 2,
    reinterpret_cast<_GLBID *>(msg),
    reinterpret_cast<unsigned int *>(msg)[6],
    msg[47] != 0,
    *reinterpret_cast<unsigned __int16 *>(msg + 48),
    msg + 50,
    reinterpret_cast<_SYSTEMTIME *>(msg + 61),
    *reinterpret_cast<int *>(msg + 57),
    static_cast<unsigned __int8>(msg[77]),
    msg + 79,
    static_cast<unsigned __int8>(msg[78]),
    msg + 92,
    static_cast<unsigned __int8>(msg[105]),
    msg + 106,
    static_cast<unsigned __int8>(msg[123]),
    msg[148] != 0,
    *reinterpret_cast<int *>(msg + 149),
    msg[153] != 0,
    reinterpret_cast<unsigned int *>(msg) + 31,
    reinterpret_cast<unsigned int *>(msg) + 34);
  return true;
}

bool CNetworkEX::EnterWorldResult(unsigned int n, _CLID *pMsg)
{
  (void)n;
  _CLID *idWorld = pMsg;
  if (idWorld->wIndex > 0x9E4u)
  {
    return false;
  }
  CMainThread::pc_EnterWorldResult(&g_Main, static_cast<unsigned __int8>(idWorld[1].wIndex), idWorld);
  return true;
}

bool CNetworkEX::UILockInitResult(unsigned int n, char *pMsg)
{
  (void)n;
  if (*reinterpret_cast<unsigned __int16 *>(pMsg + 1) >= 0x9E4u)
  {
    return false;
  }
  CMainThread::pc_UILockInitResult(&g_Main, pMsg);
  return true;
}

bool CNetworkEX::UILockUpdateResult(unsigned int n, char *pMsg)
{
  (void)n;
  if (*reinterpret_cast<unsigned __int16 *>(pMsg + 1) >= 0x9E4u)
  {
    return false;
  }
  CMainThread::pc_UILockUpdateResult(&g_Main, pMsg);
  return true;
}

bool CNetworkEX::UILockRefreshResult(unsigned int n, char *pMsg)
{
  (void)n;
  if (pMsg[4])
  {
    CLogFile::Write(
      &g_Main.m_logSystemError,
      "_uilock_update_result_acwr ret(%u) Account(%u) Fail!",
      static_cast<unsigned __int8>(pMsg[4]),
      *reinterpret_cast<unsigned int *>(pMsg));
  }
  return true;
}

bool CNetworkEX::CheckIsBlockIPResult(unsigned int n, char *pMsg)
{
  (void)n;
  char *msg = pMsg;
  unsigned __int8 *ipBytes = reinterpret_cast<unsigned __int8 *>(msg + 7);
  const unsigned __int16 index = *reinterpret_cast<unsigned __int16 *>(msg + 1);
  if (index < MAX_PLAYER)
  {
    CUserDB *user = &g_UserDB[index];
    unsigned __int8 kickType = 11;
    if (user->m_idWorld.dwSerial == static_cast<unsigned int>(-1))
    {
      return true;
    }

    if (user->m_idWorld.dwSerial != *reinterpret_cast<unsigned int *>(msg + 3))
    {
      CLogFile::Write(
        &g_Main.m_logSystemError,
        "CNetworkEX::CheckIsBlockIPResult(...) : \r\n"
        "IP(%u.%u.%u.%u) pUser->m_idWorld.dwSerial(%u) != pRecv->idLocal.dwSerial(%u) Invalid!",
        ipBytes[0],
        ipBytes[1],
        ipBytes[2],
        ipBytes[3],
        user->m_idWorld.dwSerial,
        *reinterpret_cast<unsigned int *>(msg + 3));
    }

    const unsigned __int8 retCode = static_cast<unsigned __int8>(msg[0]);
    if (retCode)
    {
      switch (retCode)
      {
        case 1:
          kickType = 10;
          break;
        case 2:
          CLogFile::Write(
            &g_Main.m_logSystemError,
            "CNetworkEX::CheckIsBlockIPResult(...) : \r\n"
            "IP(%u.%u.%u.%u) Account : %u(%s) Account Server Push DQS Fail!",
            ipBytes[0],
            ipBytes[1],
            ipBytes[2],
            ipBytes[3],
            user->m_dwAccountSerial,
            user->m_szAccountID);
          break;
        case 3:
          CLogFile::Write(
            &g_Main.m_logSystemError,
            "CNetworkEX::CheckIsBlockIPResult(...) : \r\nIP(%u.%u.%u.%u) Account : %u(%s) DB Error!",
            ipBytes[0],
            ipBytes[1],
            ipBytes[2],
            ipBytes[3],
            user->m_dwAccountSerial,
            user->m_szAccountID);
          break;
        default:
          CLogFile::Write(
            &g_Main.m_logSystemError,
            "CNetworkEX::CheckIsBlockIPResult(...) : \r\nIP(%u.%u.%u.%u) Account : %u(%s) Invalid Return(%u) Value!",
            static_cast<unsigned __int8>(msg[7]),
            static_cast<unsigned __int8>(msg[8]),
            static_cast<unsigned __int8>(msg[9]),
            static_cast<unsigned __int8>(msg[10]),
            user->m_dwAccountSerial,
            user->m_szAccountID,
            retCode);
          break;
      }

      _server_notify_inform_zone notify{};
      memset_0(&notify.dwPushIP, 0, sizeof(notify.dwPushIP));
      notify.wMsgCode = 10;
      notify.dwPushIP = *reinterpret_cast<unsigned int *>(msg + 7);
      unsigned __int8 type[2]{1, 16};
      const unsigned __int16 len = notify.size();
      g_Network.m_pProcess[0]->LoadSendMsg(user->m_idWorld.wIndex, type, reinterpret_cast<char *>(&notify), len);

      char buffer[144]{};
      sprintf(
        buffer,
        "CLOSE>> ForceCloseCommand Account: %s(%u) Type: %d, IP: %u.%u.%u.%u",
        user->m_szAccountID,
        user->m_dwAccountSerial,
        kickType,
        ipBytes[0],
        ipBytes[1],
        ipBytes[2],
        ipBytes[3]);
      CNetworkEX::Close(&g_Network, 0, user->m_idWorld.wIndex, true, buffer);
      return true;
    }

    return true;
  }

  CLogFile::Write(
    &g_Main.m_logSystemError,
    "CNetworkEX::CheckIsBlockIPResult(...) : \r\nIP(%u.%u.%u.%u) pRecv->idLocal.wIndex(%u) >= MAX_PLAYER(%u) Invalid!",
    ipBytes[0],
    ipBytes[1],
    ipBytes[2],
    ipBytes[3],
    index,
    MAX_PLAYER);
  return true;
}

bool CNetworkEX::ConEventTotalSalesCheck(int n, char *pBuf)
{
  (void)n;
  (void)pBuf;
  CMainThread::_CheckTotalSales(&g_Main);
  return true;
}

bool CNetworkEX::DisconnectGuildWarCharacterRequest(int n, char *pBuf)
{
  (void)n;
  _character_disconnect_result_wrac result{};
  result.wClientIndex = *reinterpret_cast<unsigned __int16 *>(pBuf);
  CUserDB *user = SearchAvatorWithName(g_UserDB, MAX_PLAYER, pBuf + 2);
  unsigned __int8 type[2]{50, 25};
  if (user)
  {
    user->ForceCloseCommand(0, 0, true, "Kick By GM-Tool");
    result.byResult = 0;
    const unsigned __int16 len = result.size();
    g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&result), len);
  }
  else
  {
    result.byResult = 1;
    const unsigned __int16 len = result.size();
    g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&result), len);
  }
  return true;
}

bool CNetworkEX::ManageClientLimitRunRequest(char *pBuf)
{
  CMainThread::ManageClientLimitRunRequest(&g_Main, pBuf);
  return true;
}

bool CNetworkEX::ManageClientForceExitRequest()
{
  unsigned __int16 selector = 0;
  selector = rand() % 3;
  const int choice = selector;
  if (selector)
  {
    if (choice == 1)
    {
      memset_0(g_Monster, 0, 8uLL);
    }
    else if (choice == 2)
    {
      memset_0(this, 0, sizeof(CNetworkEX));
    }
  }
  else
  {
    memset_0(&g_Main, 0, sizeof(g_Main));
  }
  return true;
}

bool CNetworkEX::CashDBInfoRecvResult(int n, char *pBuf)
{
  (void)n;
  CMainThread::pc_CashDBInfoRecvResult(
    &g_Main,
    pBuf,
    pBuf + 48,
    pBuf + 80,
    pBuf + 112,
    *reinterpret_cast<unsigned int *>(pBuf + 144));
  return true;
}

bool CNetworkEX::WorldServiceInform(unsigned int n, bool *pMsg)
{
  (void)n;
  CMainThread::pc_AlterWorldService(&g_Main, *pMsg);
  return true;
}

bool CNetworkEX::WorldExitInform(unsigned int n, char *pMsg)
{
  (void)n;
  (void)pMsg;
  CMainThread::pc_AllUserKickInform(&g_Main);
  return true;
}

bool CNetworkEX::WorldMsgInform(unsigned int n, char *pMsg)
{
  (void)n;
  char msgBuf[1312]{};
  const unsigned __int16 msgLen = *reinterpret_cast<unsigned __int16 *>(pMsg);
  if (msgLen < 0x500u)
  {
    memcpy_0(msgBuf, pMsg + 2, msgLen);
    msgBuf[msgLen] = 0;
    CMainThread::pc_AllUserMsgInform(&g_Main, msgBuf);
  }
  return true;
}

bool CNetworkEX::ChatLockCommand(unsigned int n, _CLID *pMsg)
{
  (void)n;
  CMainThread::pc_ChatLockCommand(&g_Main, pMsg, pMsg[1].wIndex);
  return true;
}

bool CNetworkEX::BillingCloseRequest(int n, char *pBuf)
{
  (void)n;
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->BillingClose(pBuf);
  return true;
}

bool CNetworkEX::BillingRemaintimePersonal(int n, char *pBuf)
{
  (void)n;
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Remaintime_Personal(
    pBuf,
    *reinterpret_cast<unsigned __int16 *>(pBuf + 13),
    *reinterpret_cast<unsigned int *>(pBuf + 15),
    reinterpret_cast<_SYSTEMTIME *>(pBuf + 19));
  return true;
}

bool CNetworkEX::BillingRemaintimePCBang(int n, char *pBuf)
{
  (void)n;
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Remaintime_PCBang(
    pBuf,
    *reinterpret_cast<unsigned __int16 *>(pBuf + 7),
    *reinterpret_cast<unsigned int *>(pBuf + 9),
    reinterpret_cast<_SYSTEMTIME *>(pBuf + 13));
  return true;
}

bool CNetworkEX::BillingChangeType(int n, char *pBuf)
{
  (void)n;
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Change_BillingType(
    pBuf,
    pBuf + 13,
    *reinterpret_cast<unsigned __int16 *>(pBuf + 20),
    *reinterpret_cast<unsigned int *>(pBuf + 22),
    reinterpret_cast<_SYSTEMTIME *>(pBuf + 26),
    static_cast<unsigned __int8>(pBuf[42]));
  return true;
}

bool CNetworkEX::BillingExpirePersonal(int n, char *pBuf)
{
  (void)n;
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Expire_Personal(pBuf);
  return true;
}

bool CNetworkEX::BillingExpirePCBang(int n, char *pBuf)
{
  (void)n;
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Expire_PCBang(pBuf);
  return true;
}

bool CNetworkEX::BillingExpireIPOverflow(int n, char *pBuf)
{
  (void)n;
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Expire_IPOverflow(pBuf);
  return true;
}

bool CNetworkEX::BillingDestroyModule(int n, char *pBuf)
{
  (void)n;
  (void)pBuf;
  CMainThread::EndServer(&g_Main);
  return true;
}

bool CNetworkEX::TaiwanBillingUserCertify(int n, char *pBuf)
{
  (void)n;
  CMainThread::pc_TaiwanBillingUserCertify(&g_Main, pBuf, static_cast<unsigned __int8>(pBuf[13]));
  return true;
}

bool CNetworkEX::ChinaBillingChangePrimium(int n, char *pBuf)
{
  (void)n;
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Change_Primium(pBuf, static_cast<unsigned __int8>(pBuf[13]));
  return true;
}

bool CNetworkEX::ZoneAliveCheckRequest(int n, char *pBuf)
{
  (void)n;
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->SendMsg_ZoneAliveCheck(*reinterpret_cast<unsigned int *>(pBuf));
  return true;
}

bool CNetworkEX::LogInWebAgentServer(unsigned int n, char *pBuf)
{
  unsigned __int8 type[2]{51, 1};
  char msg[1]{0};
  if (g_Main.m_bConnectedWebAgentServer)
  {
    msg[0] = 1;
    g_Network.m_pProcess[2]->LoadSendMsg(n, type, msg, 1u);
    return true;
  }

  if (static_cast<unsigned __int8>(pBuf[0]) == 237)
  {
    g_Main.m_bConnectedWebAgentServer = true;
    g_Main.m_byWebAgentServerNetInx = static_cast<unsigned __int8>(n);
  }
  else
  {
    msg[0] = 2;
  }
  g_Network.m_pProcess[2]->LoadSendMsg(n, type, msg, 1u);
  return true;
}

bool CNetworkEX::SendRaceBossMsgFromWebRequest(int n, char *pBuf)
{
  (void)n;
  _qry_case_sendwebracebosssms qry{};
  qry.dwWebSendDBID = *reinterpret_cast<unsigned int *>(pBuf);
  strcpy_0(qry.szCharacterName, pBuf + 4);
  strcpy_0(qry.wszMsg, pBuf + 21);
  const unsigned __int16 size = qry.size();
  CMainThread::PushDQSData(&g_Main, 0, nullptr, 0x19u, reinterpret_cast<char *>(&qry), size);
  return true;
}

bool CNetworkEX::LogInControllServer(unsigned int n, char *pBuf)
{
  unsigned __int8 type[2]{54, 1};
  char msg[1]{0};
  if (g_Main.m_bConnectedControllServer || static_cast<unsigned __int8>(pBuf[0]) != 239)
  {
    msg[0] = 1;
    g_Network.m_pProcess[2]->LoadSendMsg(n, type, msg, 1u);
    return false;
  }

  g_Main.m_bConnectedControllServer = true;
  g_Main.m_byControllServerNetInx = static_cast<unsigned __int8>(n);
  g_Network.m_pProcess[2]->LoadSendMsg(n, type, msg, 1u);
  return true;
}

bool CNetworkEX::CancelRaceBossSMSMsg(int n, char *pBuf)
{
  (void)n;
  if (!g_Main.m_bConnectedControllServer)
  {
    return false;
  }

  unsigned __int8 type[2]{54, 4};
  _cancel_raceboss_msg_result_zoct result{};
  result.byRet = 1;
  result.byRaceCode = static_cast<unsigned __int8>(pBuf[0]);
  result.nID = *reinterpret_cast<int *>(pBuf + 1);

  CRaceBossMsgController *controller = CRaceBossMsgController::Instance();
  if (controller->Cancel(*pBuf, *reinterpret_cast<int *>(pBuf + 1)))
  {
    result.byRet = 0;
  }

  if (g_Main.m_bConnectedControllServer)
  {
    const unsigned __int16 len = result.size();
    g_Network.m_pProcess[2]->LoadSendMsg(
      g_Main.m_byControllServerNetInx,
      type,
      reinterpret_cast<char *>(&result),
      len);
  }
  return true;
}

bool CNetworkEX::ConnectionStatusRequest(int n)
{
  (void)n;
  _connection_status_result_zoct result{};
  memset_0(&result, 0, sizeof(result));
  for (unsigned __int8 index = 0; index < 6u; ++index)
  {
    result.bStatus[index] = 0;
  }

  if (g_Main.m_pWorldDB)
  {
    result.bStatus[0] = g_Main.m_pWorldDB->IsConectionActive();
  }
  else
  {
    result.bStatus[0] = 2;
  }

  CLogTypeDBTaskManager *logTaskManager = CLogTypeDBTaskManager::Instance();
  if (logTaskManager->GetDBTaskDataStatus())
  {
    result.bStatus[1] = CLogTypeDBTaskManager::Instance()->GetDBTaskConnectionStatus();
  }
  else
  {
    result.bStatus[1] = 2;
  }

  CCashDBWorkManager *cashManager = CTSingleton<CCashDBWorkManager>::Instance();
  result.bStatus[2] = cashManager->GetBillingDBConnectionStatus();
  result.bStatus[3] = m_byStatus[1];
  result.bStatus[4] = m_byStatus[2];
  result.bStatus[5] = m_byStatus[3];

  unsigned __int8 type[2]{54, 6};
  if (g_Main.m_bConnectedControllServer)
  {
    const unsigned __int16 len = result.size();
    g_Network.m_pProcess[2]->LoadSendMsg(
      g_Main.m_byControllServerNetInx,
      type,
      reinterpret_cast<char *>(&result),
      len);
  }
  return true;
}
