#include "pch.h"

#include "CNetworkEX.h"

#include <cstdarg>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <string>

#include "AutoMineMachineMng.h"
#include "AutominePersonalMgr.h"
#include "CashItemRemoteStore.h"
#include "FireGuard.h"
#include "ccrfg_detect_alret.h"
#include "CBillingManager.h"
#include "CCashDBWorkManager.h"
#include "CLogTypeDBTaskManager.h"
#include "CAsyncLogger.h"
#include "CChiNetworkEX.h"
#include "CMainThread.h"
#include "CNationSettingManager.h"
#include "CNuclearBombMgr.h"
#include "CPcBangFavor.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "TrapItem_fld.h"
#include "CGuildBattleController.h"
#include "CGuild.h"
#include "CheatCommands.h"
#include "PatriarchElectProcessor.h"
#include "CPlayer.h"
#include "CPostSystemManager.h"
#include "CItemStoreManager.h"
#include "CItemStore.h"
#include "CMapData.h"
#include "CGuildRoomSystem.h"
#include "CUnmannedTraderController.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CReturnGateController.h"
#include "CUserDB.h"
#include "CRaceBossMsgController.h"
#include "CRecallEffectController.h"
#include "CTotalGuildRankManager.h"
#include "CWeeklyGuildRankManager.h"
#include "DqsDbStructs.h"
#include "DqsOnRunPayloads.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "cancel_raceboss_msg_result_zoct.h"
#include "enter_world_request_zone.h"
#include "enter_world_result_zone.h"
#include "Packet/AccountZonePacket.h"
#include "Packet/BillingZonePacket.h"
#include "Packet/ClientZonePacket.h"
#include "Packet/WebZonePacket.h"
#include "Packet/ZoneAccountPacket.h"
#include "Packet/ZoneWebPacket.h"
#include "Packet/ZoneClientPacket.h"
#include "server_notify_inform_zone.h"
#include "tuning_data.h"
#include "w_name.h"
#include "qry_case_select_timelimit_info.h"
#include "StoreList_fld.h"

namespace
{
unsigned __int8 sbyCcrFgBlock[2] = {102, 5};
_ccrfg_detect_alret sCcrFgBlock{};

void WritePacketLogLine(CLogFile &logFile, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  logFile.WriteFromArg(format, args);
  va_end(args);
}

void BuildPacketHex(std::string &out, const char *payload, unsigned int length)
{
  out.clear();
  if (!payload || !length)
  {
    out = "-";
    return;
  }

  out.reserve(static_cast<size_t>(length) * 3u);
  static const char kHex[] = "0123456789ABCDEF";
  for (unsigned int i = 0; i < length; ++i)
  {
    const unsigned char value = static_cast<unsigned char>(payload[i]);
    out.push_back(kHex[value >> 4]);
    out.push_back(kHex[value & 0x0F]);
    if (i + 1 < length)
    {
      out.push_back(' ');
    }
  }
}

void AppendPacketSnifferLogRecv(unsigned int socketIndex, _MSG_HEADER *header, char *payload)
{
  if (!header || header->m_byType[0] >= 100u)
  {
    return;
  }

  const unsigned int payloadLen = (header->m_wSize >= 4u) ? static_cast<unsigned int>(header->m_wSize - 4u) : 0u;
  if (!payload || !payloadLen)
  {
    WritePacketLogLine(
      g_Network.m_LogFile,
      "PKT RECV idx(%u) type(%u,%u) len(%u) packet(-)",
      socketIndex,
      static_cast<unsigned int>(header->m_byType[0]),
      static_cast<unsigned int>(header->m_byType[1]),
      payloadLen);
    return;
  }

  std::string packetHex;
  BuildPacketHex(packetHex, payload, payloadLen);
  WritePacketLogLine(
    g_Network.m_LogFile,
    "PKT RECV idx(%u) type(%u,%u) len(%u) packet(%s)",
    socketIndex,
    static_cast<unsigned int>(header->m_byType[0]),
    static_cast<unsigned int>(header->m_byType[1]),
    payloadLen,
    packetHex.c_str());
}
}

void DeCrypt_Move(char *pStr, int nSize, unsigned __int8 byPlus, unsigned __int16 wCryptKey)
{
  for (int index = 0; index < nSize; ++index)
  {
    unsigned char value = static_cast<unsigned char>(*pStr);
    value ^= static_cast<unsigned char>(wCryptKey + 32);
    value = static_cast<unsigned char>(value - static_cast<unsigned char>(byPlus + 23));
    *pStr = static_cast<char>(value);
    ++pStr;
  }
}

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

CNetworkEX::CNetworkEX()
{
  dwPlayerPosMiss = 0;
  dwMonsterPosMiss = 0;
  m_byStatus[0] = 0;
  m_byStatus[1] = 0;
  m_byStatus[2] = 0;
  m_byStatus[3] = 0;
}

CNetworkEX::~CNetworkEX()
{
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
  wsprintfA(szFileName, "%s\\%s_Sys%u.log", this->m_szLogPath, this->m_szSystemName, korLocalTime);
  this->m_LogFile.SetWriteLogFile(szFileName, 1, 1, 1, 1);

  const unsigned int fgLogTime = GetKorLocalTime();
  wsprintfA(szFileName, "%s\\%s_CcrFgSys%u.log", this->m_szLogPath, this->m_szSystemName, fgLogTime);
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
// this is not a stub
  return true;
}

void CNetWorking::AcceptClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial)
{
// this is not a stub
}

void CNetWorking::CloseClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial)
{
// this is not a stub
}

bool CNetWorking::ExpulsionSocket(unsigned int dwProID, unsigned int dwIndex, unsigned __int8 byReason, void *pvInfo)
{
CloseSocket(dwProID, dwIndex, false);
  return true;
}

bool CNetworkEX::ExpulsionSocket(unsigned int dwProID, unsigned int dwIndex, unsigned __int8 byReason, void *pvInfo)
{
  if (dwProID != 0)
  {
    return true;
  }

  if (byReason == 3)
  {
    CUserDB *user = &g_UserDB[dwIndex];
    char accountId[48]{};
    strcpy_0(accountId, "null");
    if (user->m_bActive)
    {
      strcpy_0(accountId, user->m_szAccountID);
    }

    _socket *socket = GetSocket(0, dwIndex);
    const char *ip = socket ? inet_ntoa(socket->m_Addr.sin_addr) : "0.0.0.0";
    char logBuffer[136]{};
    sprintf(logBuffer, "CLOSE>> ExpulsionSocket[Buffer: Send Buffer Full] ID: %s, IP: %s", accountId, ip);
    Close(0, dwIndex, false, logBuffer);
    return true;
  }

  if (byReason == 0)
  {
    CUserDB *user = &g_UserDB[dwIndex];
    char accountId[48]{};
    strcpy_0(accountId, "null");
    if (user->m_bActive)
    {
      strcpy_0(accountId, user->m_szAccountID);
    }

    _socket *socket = GetSocket(0, dwIndex);
    const char *ip = socket ? inet_ntoa(socket->m_Addr.sin_addr) : "0.0.0.0";
    char logBuffer[136]{};
    sprintf(logBuffer, "CLOSE>> ExpulsionSocket[SpeedHack: No Respon] ID: %s, IP: %s", accountId, ip);
    Close(0, dwIndex, false, logBuffer);
    return true;
  }

  switch (byReason)
  {
    case 1:
    {
      auto *info = reinterpret_cast<unsigned __int16 *>(pvInfo);
      _socket *socket = GetSocket(0, dwIndex);
      if (!socket || !socket->m_bAccept)
      {
        return false;
      }

      const unsigned __int8 *raw = reinterpret_cast<const unsigned __int8 *>(info);
      if (raw[2] == 1 && raw[3] == 1)
      {
        socket->m_bEnterCheck = true;
        return false;
      }

      const char *ip = inet_ntoa(socket->m_Addr.sin_addr);
      char logBuffer[136]{};
      sprintf(
        logBuffer,
        "CLOSE>> ExpulsionSocket[Certify None: %d,%d, size:%d] IP: %s",
        raw[2],
        raw[3],
        *info,
        ip);
      Close(0, dwIndex, false, logBuffer);
      return true;
    }
    case 2:
    {
      _socket *socket = GetSocket(0, dwIndex);
      if (!socket || !socket->m_bAccept)
      {
        return false;
      }

      const char *ip = inet_ntoa(socket->m_Addr.sin_addr);
      char logBuffer[136]{};
      sprintf(logBuffer, "CLOSE>> ExpulsionSocket[Certify Delay] IP: %s", ip);
      Close(0, dwIndex, false, logBuffer);
      return true;
    }
    case 5:
    {
      if (!m_bUseFG)
      {
        return true;
      }

      _socket *socket = GetSocket(0, dwIndex);
      if (!socket || !socket->m_bAccept)
      {
        return false;
      }

      CUserDB *user = &g_UserDB[dwIndex];
      const int fgStatus = *reinterpret_cast<int *>(pvInfo);
      if (!user->m_bActive)
      {
        Close(0, dwIndex, true, "FireGuard Detect !pUser->m_bActive !!");
        return true;
      }

      CAsyncLogger::Instance()->FormatLog(
        7,
        "IP(%s) : Account(%s) : AccountSerial(%u) : FGStatus(%#x)",
        inet_ntoa(socket->m_Addr.sin_addr),
        user->m_szAccountID,
        user->m_dwAccountSerial,
        fgStatus);

      if (fgStatus < 11 || fgStatus > 14)
      {
        CAsyncLogger::Instance()->FormatLog(
          7,
          "Fire Guard Punishment type Invaild : Type(%#x)",
          fgStatus);
      }
      else
      {
        const unsigned __int16 packetLen = sCcrFgBlock.size();
        g_Network.m_pProcess[0]->LoadSendMsg(
          dwIndex,
          sbyCcrFgBlock,
          reinterpret_cast<char *>(&sCcrFgBlock),
          packetLen);
        Close(0, dwIndex, true, nullptr);
      }
      return true;
    }
    default:
      return true;
  }
}

void CNetWorking::AnsyncConnectComplete(unsigned int dwProID, unsigned int dwIndex, int nResult)
{
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
  AppendPacketSnifferLogRecv(n, pMsgHeader, pMsg);

  bool result = false;
  const int type0 = pMsgHeader->m_byType[0];
  switch (type0 - 1)
  {
    case 0:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = this->EnterWorldRequest(n, pMsgHeader, pMsg);
          break;
        case 5:
          result = this->MoveLobbyRequest(n, pMsg);
          break;
        case 7:
          result = this->RegedCharRequest(n, pMsg);
          break;
        case 9:
          result = this->AddCharRequest(n, pMsg);
          break;
        case 11:
          result = this->DelCharRequest(n, pMsg);
          break;
        case 13:
          result = this->SelCharRequest(n, pMsg);
          break;
        case 16:
          result = this->TutorialProcessReport(n, pMsg);
          break;
        case 21:
          result = this->AliveCharRequest(n, pMsg);
          break;
        case 25:
          result = this->NotifyLocalTimeRequest(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 1:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = this->ChatOperatorRequest(n, pMsg);
          break;
        case 1:
          result = this->ChatCircleRequest(n, pMsg);
          break;
        case 2:
          result = this->ChatFarRequest(n, pMsg);
          break;
        case 4:
          result = this->ChatPartyRequest(n, pMsg);
          break;
        case 5:
          result = this->ChatRaceRequest(n, pMsg);
          break;
        case 6:
          result = this->ChatCheatRequest(n, pMsg);
          break;
        case 7:
          result = this->ChatManageRequest(n, pMsg);
          break;
        case 8:
          result = this->ChatMgrWhisperRequest(n, pMsg);
          break;
        case 9:
          result = this->ChatMapRecvYesOrNo(n, pMsg);
          break;
        case 10:
          result = this->ChatMapRequest(n, pMsg);
          break;
        case 11:
          result = this->ChatRaceBossRequest(n, pMsg);
          break;
        case 12:
          result = this->ChatGuildEstSenRequest(n, pMsg);
          break;
        case 13:
          result = this->ChatRePresentationRequest(n, pMsg);
          break;
        case 14:
          result = this->ChatAllRecvYesOrNo(n, pMsg);
          break;
        case 15:
          result = this->ChatAllRequest(n, pMsg);
          break;
        case 16:
          result = this->ChatGreetingMsg_GM(n, pMsg);
          break;
        case 17:
          result = this->ChatGreetingMsg_RACE(n, pMsg);
          break;
        case 18:
          result = this->ChatGreetingMsg_GUILD(n, pMsg);
          break;
        case 19:
          result = this->ChatTradeRequestMsg(n, pMsg);
          break;
        case 98:
          result = this->ChatGuildRequest(n, pMsg);
          break;
        case 100:
          result = this->ChatMultiFarRequest(n, pMsg);
          break;
        case 103:
          result = this->ChatRaceBossCryRequest(n, pMsg);
          break;
        case 104:
          result = this->ChatGmNoticeRequest(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 2:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = this->NewPosStartRequest(n, reinterpret_cast<_new_pos_start_request_clzo *>(pMsg));
          break;
        case 2:
          result = this->BaseDownloadRequest(n, pMsg);
          break;
        case 4:
          result = this->InvenDownloadRequest(n, pMsg);
          break;
        case 6:
          result = this->CumDownloadRequest(n, pMsg);
          break;
        case 8:
          result = this->ForceDownloadRequest(n, pMsg);
          break;
        case 10:
          result = this->QuestDownloadRequest(n, pMsg);
          break;
        case 12:
          result = this->SpecialDownloadRequest(n, pMsg);
          break;
        case 29:
          result = this->OtherShapeRequest(n, pMsg);
          break;
        case 36:
          result = this->Revival(n, pMsg);
          break;
        case 40:
          result = this->ExitWorldRequest(n, pMsg);
          break;
        case 45:
          result = this->LinkBoardDownloadRequest(n, pMsg);
          break;
        case 48:
          result = this->MacroDownLoadRequest(n, pMsg);
          break;
        case 53:
          result = this->AMP_DownloadRequest(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 3:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = this->NextPoint(n, pMsg);
          break;
        case 1:
          result = this->RealMovPosRequest(n, pMsg);
          break;
        case 18:
          result = this->Stop(n, pMsg);
          break;
        case 25:
          result = this->GotoBasePortalRequest(n, pMsg);
          break;
        case 27:
          result = this->GotoAvatorRequest(n, reinterpret_cast<_goto_avator_request_clzo *>(pMsg));
          break;
        case 30:
          result = this->MoveTypeChangeRequeset(n, pMsg);
          break;
        case 31:
          result = this->MoveInfoRequeset(n, pMsg);
          break;
        case 32:
          result = this->PlayerInfoResult(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 4:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = this->AttackPersonalRequest(n, pMsg);
          break;
        case 1:
          result = this->AttackSkillRequest(n, pMsg);
          break;
        case 2:
          result = this->AttackForceRequest(n, pMsg);
          break;
        case 3:
          result = this->AttackUnitRequest(n, pMsg);
          break;
        case 4:
          result = this->AttackTestRequest(n, pMsg);
          break;
        case 54:
          result = this->AttackSiegeRequest(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 6:
    {
      const int subType = static_cast<int>(pMsgHeader->m_byType[1]) - 2;
      switch (subType)
      {
        case 0:
          result = this->ItemboxTakeRequest(n, pMsg);
          break;
        case 3:
          result = this->ThrowStorageRequest(n, reinterpret_cast<_throw_storage_request_clzo *>(pMsg));
          break;
        case 5:
          result = this->UsePotionRequest(n, pMsg);
          break;
        case 7:
          result = this->EquipPartRequest(n, reinterpret_cast<_equip_part_request_clzo *>(pMsg));
          break;
        case 9:
          result = this->EmbellishRequest(n, pMsg);
          break;
        case 11:
          result = this->OffPartRequest(n, reinterpret_cast<_off_part_request_clzo *>(pMsg));
          break;
        case 13:
          result = this->MakeItemRequest(n, pMsg);
          break;
        case 15:
          result = this->UpgradeItemRequest(n, pMsg);
          break;
        case 17:
          result = this->DownGradeItemRequest(n, pMsg);
          break;
        case 19:
          result = this->AddBagRequest(n, pMsg);
          break;
        case 24:
          result = this->UseRecoverLossExpItemRequest(n, pMsg);
          break;
        case 26:
          result = this->CombineItemRequest(n, pMsg);
          break;
        case 28:
          result = this->ExchangeItemRequest(n, pMsg);
          break;
        case 30:
          result = this->CombineExItemRequest(n, pMsg);
          break;
        case 33:
          result = this->CombineExItemAcceptRequest(n, pMsg);
          break;
        case 35:
          result = this->UseFireCrackerItemRequest(n, pMsg);
          break;
        case 43:
          result = this->SetItemCheckRequest(n, pMsg);
          break;
        case 44:
          result = this->UseSoccerBallItemRequest(n, pMsg);
          break;
        case 47:
          result = this->UseRadarItemRequest(n, pMsg);
          break;
        case 49:
          result = this->RadarCharListRequest(n, pMsg);
          break;
        case 57:
          result = this->NPCLinkCheckItemRequest(n, pMsg);
          break;
        case 61:
          result = this->UseRecallTeleportItemRequest(n, reinterpret_cast<_use_recall_teleport_item_request_clzo *>(pMsg));
          break;
        case 64:
          result = this->CharacterRenameCash(n, pMsg);
          break;
        case 73:
          result = this->TalikCrystalExchangeRequest(n, pMsgHeader, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 7:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      if (subType == 1)
      {
        result = this->MovePortalRequest(n, pMsg);
      }
      else if (subType == 3)
      {
        result = this->RegistBindRequest(n, pMsg);
      }
      else
      {
        result = subType == 9 && this->EnterReturnGateRequest(n, pMsg);
      }
      break;
    }
    case 10:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      switch (subType)
      {
        case 6u:
          result = this->SelectClassRequest(n, pMsg);
          break;
        case 0x18u:
          result = this->InitClassRequest(n, reinterpret_cast<_init_class_request_clzo *>(pMsg));
          break;
        case 0x1Au:
          result = this->InitClassCostRequest(n, pMsg);
          break;
        case 0x1Cu:
          result = this->CanSelectClassRequest(n, pMsg);
          break;
        default:
          result = subType == 33 && this->SelectPcBangRewardRequest(n, pMsg);
          break;
      }
      break;
    }
    case 11:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = this->BuyStoreRequest(n, pMsg);
          break;
        case 3:
          result = this->SellStoreRequest(n, pMsg);
          break;
        case 7:
          result = this->StoreListRequest(n, pMsg);
          break;
        case 9:
          result = this->ExchangeDalantForGoldRequest(n, pMsg);
          break;
        case 10:
          result = this->ExchangeGoldForDalantRequest(n, pMsg);
          break;
        case 16:
          result = this->LimitItemNumRequest(n, pMsg);
          break;
        case 18:
          result = this->TalikRecorverList(n, pMsg);
          break;
        case 20:
          result = this->PvpCashRecorverWithTalik(n, pMsg);
          break;
        case 22:
          result = this->PcBangPrimiumCouponRequest(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 12:
    {
      const int subType = static_cast<int>(pMsgHeader->m_byType[1]) - 2;
      switch (subType)
      {
        case 0:
          result = ForceInvenChangeRequest(n, pMsg);
          break;
        case 2:
          result = AnimusInvenChangeRequest(n, pMsg);
          break;
        case 4:
          result = ResSeparationRequest(n, pMsg);
          break;
        case 6:
          result = ResDivisionRequest(n, pMsg);
          break;
        case 10:
          result = PotionSocketSeparationRequest(n, pMsg);
          break;
        case 12:
          result = PotionSocketDivisionRequest(n, pMsg);
          break;
        case 14:
          result = AlterItemSlotRequest(n, pMsg);
          break;
        case 15:
          result = AlterLinkBoardSlotRequest(n, pMsg);
          break;
        case 16:
          result = PvpRankListRequest(n, pMsg);
          break;
        case 19:
          result = ModeChangeRequest(n, pMsg);
          break;
        case 20:
          result = GustureRequest(n, pMsg);
          break;
        case 23:
          result = AlterWindowInfoRequest(n, pMsg);
          break;
        case 24:
          result = SetTargetObjectRequest(n, pMsg);
          break;
        case 26:
          result = ReleaseTargetObjectRequest(n, pMsg);
          break;
        case 28:
          result = PartyReqBlockReport(n, reinterpret_cast<_party_req_block_report_clzo *>(pMsg));
          break;
        case 29:
          result = WhisperBlockReport(n, reinterpret_cast<_whisper_block_report_clzo *>(pMsg));
          break;
        case 30:
          result = TradeBlockReport(n, reinterpret_cast<_trade_block_report_clzo *>(pMsg));
          break;
        case 33:
          result = GuildBattleBlockReport(n, reinterpret_cast<_guild_battle_block_report_clzo *>(pMsg));
          break;
        case 34:
          result = PlayerMacroUpdate(n, pMsg);
          break;
        case 35:
          result = TotalGuildRankRequest(n, pMsg);
          break;
        case 37:
          result = WeeklyGuildRankRequest(n, pMsg);
          break;
        case 101:
          result = SetRaceBossCryMsgRequest(n, pMsg);
          break;
        case 102:
          result = NotifyRaceBossCryMsg(n, pMsg);
          break;
        case 104:
          result = SetGroupTargetObjectRequest(n, pMsg);
          break;
        case 106:
          result = ReleaseGroupTargetObjectRequest(n, pMsg);
          break;
        case 111:
          result = SetGroupMapPointRequest(n, pMsg);
          break;
        case 113:
          result = RequestPatriarchPunishment(n, pMsg);
          break;
        case 116:
          result = RequestTaxRate(n, pMsg);
          break;
        case 118:
          result = RequestChangeTaxRate(n, pMsg);
          break;
        case 125:
          result = RequestUILockInit(n, pMsg);
          break;
        case 127:
          result = RequestUILockUserCertify(n, pMsg);
          break;
        case 129:
          result = RequestUILockUpdateInfo(n, pMsg);
          break;
        case 132:
          result = RequestUILockFindPW(n, pMsg);
          break;
        case 143:
          result = RequestTLLogoutTime(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 13:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = MineStartRequest(n, pMsg);
          break;
        case 2:
          result = MineCancleRequest(n, pMsg);
          break;
        case 8:
          result = ResCuttingRequest(n, pMsg);
          break;
        case 10:
          result = OreIntoBagRequest(n, pMsg);
          break;
        case 12:
          result = CuttingCompleteRequest(n, pMsg);
          break;
        case 14:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = autoMine->OpenUI(n);
          break;
        }
        case 16:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = autoMine->CloseUI(n);
          break;
        }
        case 17:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = autoMine->StartWorkMachine(n);
          break;
        }
        case 18:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = autoMine->StopWorkMachine(n);
          break;
        }
        case 19:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = autoMine->SelectOreType(n, reinterpret_cast<_pt_automine_selectore_clzo *>(pMsg));
          break;
        }
        case 20:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = autoMine->GetOutOre(n, reinterpret_cast<_pt_automine_getoutore_clzo *>(pMsg));
          break;
        }
        case 22:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = autoMine->MoveOrePos(n, reinterpret_cast<_pt_automine_moveore_clzo *>(pMsg));
          break;
        }
        case 23:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = autoMine->BatteryCharge(n, reinterpret_cast<_pt_automine_charge_clzo *>(pMsg));
          break;
        }
        case 25:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = autoMine->OreMerge(n, reinterpret_cast<_pt_automine_merge_clzo *>(pMsg));
          break;
        }
        case 41:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = autoMine->make_storagebox(n, reinterpret_cast<_personal_amine_make_storage_clzo *>(pMsg));
          break;
        }
        case 43:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = autoMine->install(n, reinterpret_cast<_personal_automine_install_clzo *>(pMsg));
          break;
        }
        case 45:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = autoMine->uninstall(n, reinterpret_cast<_personal_automine_uninstall_clzo *>(pMsg));
          break;
        }
        case 47:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = autoMine->selectore(n, reinterpret_cast<_personal_automine_selore_clzo *>(pMsg));
          break;
        }
        case 49:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = autoMine->insert_battery(n, reinterpret_cast<_personal_automine_battery_insert_clzo *>(pMsg));
          break;
        }
        case 51:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = autoMine->extract_battery(n, reinterpret_cast<_personal_automine_battery_extract_clzo *>(pMsg));
          break;
        }
        case 55:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = autoMine->pop_ore(n, reinterpret_cast<_personal_automine_popore_clzo *>(pMsg));
          break;
        }
        case 60:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = autoMine->Open_InvenUI(n, reinterpret_cast<_personal_amine_infoui_open_clzo *>(pMsg));
          break;
        }
        case 62:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = autoMine->Open_InfoUI(n, reinterpret_cast<_personal_amine_infoui_open_clzo *>(pMsg));
          break;
        }
        default:
          result = false;
          break;
      }
      break;
    }
    case 15:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = PartyJoinInvitation(n, pMsg);
          break;
        case 2:
          result = PartyJoinInvitationAnswer(n, reinterpret_cast<_party_join_invitation_answer_clzo *>(pMsg));
          break;
        case 3:
          result = PartyJoinApplication(n, pMsg);
          break;
        case 5:
          result = PartyJoinApplicatiohAnswer(n, reinterpret_cast<_party_join_application_answer_clzo *>(pMsg));
          break;
        case 8:
          result = PartyLeaveSelfRequest(n, pMsg);
          break;
        case 10:
          result = PartyLeaveCompulsionRequest(n, pMsg);
          break;
        case 12:
          result = PartyDisjointRequest(n, pMsg);
          break;
        case 14:
          result = PartySuccessionRequest(n, pMsg);
          break;
        case 16:
          result = PartyLockRequest(n, reinterpret_cast<_party_lock_request_clzo *>(pMsg));
          break;
        case 27:
          result = AlterPartyLootShareRequest(n, pMsg);
          break;
        case 30:
          result = AwayPartyInvitation(n, reinterpret_cast<_away_party_invitation_request_clzo *>(pMsg));
          break;
        case 33:
          result = AwayPartyInvitationAnswer(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 16:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = ForceRequest(n, pMsg);
          break;
        case 3:
          result = SkillRequest(n, pMsg);
          break;
        case 6:
          result = ClassSkillRequest(n, pMsg);
          break;
        case 17:
          result = MakeTowerRequest(n, pMsg);
          break;
        case 19:
          result = BackTowerRequest(n, pMsg);
          break;
        case 26:
          result = MakeTrapRequest(n, pMsg);
          break;
        case 33:
          result = DecideRecallRequest(n, pMsg);
          break;
        case 37:
          result = BackTrapRequest(n, pMsg);
          break;
        case 39:
          result = ForceRecallTeleportRequest(n, pMsg);
          break;
        case 40:
          result = SkillRecallTeleportRequest(n, pMsg);
          break;
        case 41:
          result = ClassSkillRecallTeleportRequest(n, pMsg);
          break;
        case 98:
          result = ThrowSkillRequest(n, pMsg);
          break;
        case 101:
          result = ThrowUnitRequest(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 17:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = DTradeAskRequest(n, pMsg);
          break;
        case 3:
          result = DTradeAnswerRequest(n, reinterpret_cast<_d_trade_answer_request_clzo *>(pMsg));
          break;
        case 6:
          result = DTradeCancleRequest(n, pMsg);
          break;
        case 9:
          result = DTradeLockRequest(n, pMsg);
          break;
        case 12:
          result = DTradeAddRequest(n, pMsg);
          break;
        case 15:
          result = DTradeDelRequest(n, pMsg);
          break;
        case 18:
          result = DTradeBetRequest(n, pMsg);
          break;
        case 21:
          result = DTradeOKRequest(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 19:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      result = subType == 1 && ObjectServerPosRequest(n, pMsg);
      break;
    }
    case 21:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      switch (subType)
      {
        case 1u:
          result = AnimusRecallRequest(n, pMsg);
          break;
        case 3u:
          result = AnimusReturnRequest(n, pMsg);
          break;
        case 5u:
          result = AnimusCommandRequest(n, pMsg);
          break;
        default:
          result = subType == 7 && AnimusTargetRequest(n, pMsg);
          break;
      }
      break;
    }
    case 22:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = UnitFrameBuyRequest(n, pMsg);
          break;
        case 2:
          result = UnitSellRequest(n, pMsg);
          break;
        case 4:
          result = UnitPartTuningRequest(n, pMsg);
          break;
        case 6:
          result = UnitFrameRepairRequest(n, pMsg);
          break;
        case 8:
          result = UnitBulletFillRequest(n, pMsg);
          break;
        case 10:
          result = UnitPackFillRequest(n, pMsg);
          break;
        case 12:
          result = UnitDeliveryRequest(n, pMsg);
          break;
        case 14:
          result = UnitReturnRequest(n, pMsg);
          break;
        case 16:
          result = UnitTakeRequest(n, pMsg);
          break;
        case 18:
          result = UnitLeaveRequest(n, pMsg);
          break;
        case 22:
          result = UnitBulletReplaceRequest(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 23:
    {
      const int subType = static_cast<int>(pMsgHeader->m_byType[1]) - 3;
      switch (subType)
      {
        case 0:
          result = SelectWaitedQuestReport(n, pMsg);
          break;
        case 6:
          result = QuestSelectRewardReport(n, pMsg);
          break;
        case 7:
          result = NPCDialogRequest(n, pMsg);
          break;
        case 8:
          result = NPCWatchingRequest(n, pMsg);
          break;
        case 11:
          result = BriefPassReport(n, pMsg);
          break;
        case 14:
          result = NPCQuestRequest(n, pMsg);
          break;
        case 17:
          result = NPCQuestListRequest(n, pMsg);
          break;
        case 46:
          result = QuestGiveupRequest(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 24:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      result = subType == 24 && MoveToOwnStoneMapRequest(n, pMsg);
      break;
    }
    case 25:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      if (subType == 1)
      {
        result = ProposeVoteRequest(n, pMsg);
      }
      else
      {
        result = subType == 5 && CastVoteRequest(n, pMsg);
      }
      break;
    }
    case 26:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = GuildEstablishRequest(n, reinterpret_cast<_guild_establish_request_clzo *>(pMsg));
          break;
        case 4:
          result = GuildDownloadRequest(n, pMsg);
          break;
        case 5:
          result = GuildJoinApplyRequest(n, reinterpret_cast<_guild_join_apply_request_clzo *>(pMsg));
          break;
        case 9:
          result = GuildJoinApplyCancelRequest(n, pMsg);
          break;
        case 12:
          result = GuildJoinAcceptRequest(n, pMsg);
          break;
        case 15:
          result = GuildSelfLeaveRequest(n, pMsg);
          break;
        case 18:
          result = GuildOfferSuggestRequest(n, pMsg);
          break;
        case 20:
          result = GuildCancelSuggestRequest(n, pMsg);
          break;
        case 24:
          result = GuildVoteRequest(n, pMsg);
          break;
        case 32:
          result = GuildQueryInfoRequest(n, pMsg);
          break;
        case 34:
          result = GuildPushMoneyRequest(n, pMsg);
          break;
        case 46:
          result = GuildBattlePossibleGuildBattleList(n, pMsg);
          break;
        case 49:
          result = GuildBattleRankListRequest(n, pMsg);
          break;
        case 55:
          result = GuildBattleJoinGuildBattleRequest(n, pMsg);
          break;
        case 57:
          result = GuildBattleReservedScheduleRequest(n, pMsg);
          break;
        case 61:
          result = GuildBattleCurrentBattleInfoRequest(n, pMsg);
          break;
        case 69:
          result = GuildBattleTakeGravityStoneRequest(n, pMsg);
          break;
        case 70:
          result = GuildBattleGetGravityStoneRequest(n, pMsg);
          break;
        case 72:
          result = GuildBattleGoalRequest(n, pMsg);
          break;
        case 101:
          result = GuildRoomRentRequest(n, pMsg);
          break;
        case 103:
          result = GuildRoomEnterRequest(n, pMsg);
          break;
        case 105:
          result = GuildRoomOutRequest(n, pMsg);
          break;
        case 107:
          result = GuildRoomRestTimeRequest(n, pMsg);
          break;
        case 110:
          result = GuildHonorListRequest(n, pMsg);
          break;
        case 112:
          result = GuildSetHonorRequest(n, pMsg);
          break;
        case 114:
          result = GuildListRequest(n, pMsg);
          break;
        case 117:
          result = GuildNextHonorListRequest(n, pMsg);
          break;
        case 122:
          result = GuildManageRequest(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 27:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      if (subType == 1)
      {
        result = TransformSiegeModeRequest(n, pMsg);
      }
      else
      {
        result = subType == 4 && ReleaseSiegeModeRequest(n, pMsg);
      }
      break;
    }
    case 28:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      result = subType == 4 && BillingInfoRequest(n, pMsg);
      break;
    }
    case 29:
    {
      const int subType = static_cast<int>(pMsgHeader->m_byType[1]) - 3;
      switch (subType)
      {
        case 0:
          result = ATradeRegItemRequest(n, pMsg);
          break;
        case 6:
          result = ATradeAdjustPriceRequest(n, pMsg);
          break;
        case 8:
          result = ATradeClearItemRequest(n, pMsg);
          break;
        case 20:
          result = ATradeTaxRateRequest(n, pMsg);
          break;
        case 27:
          result = ATradeBuyItemRequest(n, pMsg);
          break;
        case 29:
          result = ATradeRegedListRequest(n, pMsg);
          break;
        case 31:
          result = ATradeReRegistRequest(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 30:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      switch (subType)
      {
        case 3u:
          result = BuddyDownloadRequest(n, pMsg);
          break;
        case 8u:
          result = BuddyAddRequest(n, pMsg);
          break;
        case 0xCu:
          result = BuddyAddAnswer(n, pMsg);
          break;
        default:
          result = subType == 15 && BuddyDelRequest(n, pMsg);
          break;
      }
      break;
    }
    case 32:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      result = subType == 5 && TransShipRenewTicketRequest(n, pMsg);
      break;
    }
    case 33:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = TrunkEstRequest(n, pMsg);
          break;
        case 2:
          result = TrunkDownloadRequest(n, pMsg);
          break;
        case 4:
          result = TrunkChangePasswdRequest(n, pMsg);
          break;
        case 6:
          result = TrunkExtendRequest(n, pMsg);
          break;
        case 8:
          result = TrunkAlterItemSlotRequest(n, pMsg);
          break;
        case 9:
          result = TrunkResDivisionRequest(n, pMsg);
          break;
        case 11:
          result = TrunkPotionDivisionRequest(n, pMsg);
          break;
        case 13:
          result = TrunkIoMoveRequest(n, pMsg);
          break;
        case 14:
          result = TrunkIoSwapRequest(n, pMsg);
          break;
        case 15:
          result = TrunkIoMergeRequest(n, pMsg);
          break;
        case 17:
          result = TrunkIoMoneyRequest(n, pMsg);
          break;
        case 19:
          result = TrunkPwHintIndexRequest(n, pMsg);
          break;
        case 21:
          result = TrunkHintAnswerRequest(n, reinterpret_cast<_trunk_hint_answer_request_clzo *>(pMsg));
          break;
        case 23:
          result = TrunkCreateCostIsFreeRequest(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 34:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      switch (subType)
      {
        case 0xC9:
          result = DarkHoleOpenRequest(n, pMsg);
          break;
        case 0xCB:
          result = DarkHoleEnterRequest(n, pMsg);
          break;
        case 0xCD:
          result = DarkHoleGiveupOutRequest(n, pMsg);
          break;
        case 0xCF:
          result = DarkHoleClearOutRequest(n, pMsg);
          break;
        default:
          result = subType != 0xD2 || DarkHoleAnswerReenterRequest(n, pMsg);
          break;
      }
      break;
    }
    case 51:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      result = subType == 1 && BossSMSMsgRequest(n, pMsg);
      break;
    }
    case 55:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = processor->Doit(_eRequestCandidateList, pOne, pMsg);
          break;
        }
        case 2:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = processor->Doit(_eRegCandidate, pOne, pMsg);
          break;
        }
        case 3:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = processor->Doit(_eVote, pOne, pMsg);
          break;
        }
        case 7:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = processor->Doit(_eQryAppoint, pOne, pMsg);
          break;
        }
        case 9:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = processor->Doit(_eReqAppoint, pOne, pMsg);
          break;
        }
        case 11:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = processor->Doit(_eRespAppoint, pOne, pMsg);
          break;
        }
        case 14:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = processor->Doit(_eReqDischarge, pOne, pMsg);
          break;
        }
        case 15:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = processor->Doit(_eReqPatriarchInform, pOne, pMsg);
          break;
        }
        default:
          result = false;
          break;
      }
      break;
    }
    case 56:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      CashItemRemoteStore *cashStore = CashItemRemoteStore::Instance();
      if (subType == 1)
      {
        result = cashStore->GoodsList(static_cast<unsigned __int16>(n), pMsg);
      }
      else if (subType == 3)
      {
        result = cashStore->Buy(static_cast<unsigned __int16>(n), pMsg);
      }
      else
      {
        result = false;
      }
      break;
    }
    case 57:
    {
      const int subType = pMsgHeader->m_byType[1];
      switch (subType - 1)
      {
        case 0:
          result = PostSendRequest(n, pMsg);
          break;
        case 2:
          result = PostListRequest(n, pMsg);
          break;
        case 4:
          result = PostContentRequest(n, pMsg);
          break;
        case 8:
          result = PostItemGoldRequest(n, pMsg);
          break;
        case 10:
          result = PostDeleteRequest(n, pMsg);
          break;
        case 13:
          result = PostReturnConfirmRequest(n, pMsg);
          break;
        default:
          result = false;
          break;
      }
      break;
    }
    case 59:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      if (subType == 1)
      {
        CNuclearBombMgr *mgr = CNuclearBombMgr::Instance();
        result = mgr->Request_EnableNuclearControl(n, pMsg);
      }
      else if (subType == 2)
      {
        CNuclearBombMgr *mgr = CNuclearBombMgr::Instance();
        result = mgr->Request_SelectDropPosition(n, reinterpret_cast<float *>(pMsg));
      }
      else
      {
        result = false;
      }
      break;
    }
    case 97:
    {
      CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
      result = manager->RecvGameGuardData(n, pMsgHeader, pMsg);
      break;
    }
    case 98:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      if (subType == 1)
      {
        result = Apex_R(n, pMsgHeader->m_wSize, pMsg);
      }
      else
      {
        result = subType == 2 && Apex_T(n, pMsgHeader->m_wSize, pMsg);
      }
      break;
    }
    default:
      result = false;
      break;
  }
  return result;
}

char CNetworkEX::Apex_R(int n, unsigned __int16 wSize, char *pBuf)
{
  auto *request = reinterpret_cast<_apex_result_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player)
  {
    CChiNetworkEX::Instance()->Send_Trans(
      player,
      static_cast<int>(request->dwResult));
  }
  else
  {
    CAsyncLogger::Instance()->FormatLog(12, "Apex_R - !pOne:g_Player[%d]", n);
  }
  return 1;
}

char CNetworkEX::Apex_T(int n, unsigned __int16 wSize, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (player)
  {
    CChiNetworkEX::Instance()->Send_ClienInform(player, wSize, pBuf);
  }
  else
  {
    CAsyncLogger::Instance()->FormatLog(12, "Apex_T - !pOne : g_Player[%d]", n);
  }
  return 1;
}

bool CNetworkEX::NewPosStartRequest(unsigned int n, const _new_pos_start_request_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }

  const unsigned __int8 mapInMode = request->byMapInMode;
  if (mapInMode < 9u)
  {
    player->pc_NewPosStart();
    return true;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: NewPosStartRequest()..  if(pRecv->byMapInMode >= mapin_type_num)",
    charName);
  return false;
}

bool CNetworkEX::BaseDownloadRequest(unsigned int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bBaseDownload)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: BaseDownloadRequest()..  if(pOne->m_bBaseDownload)",
      charName);
    return false;
  }

  player->SendMsg_BaseDownloadResult();
  return true;
}

bool CNetworkEX::InvenDownloadRequest(unsigned int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bInvenDownload)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: InvenDownloadRequest()..  if(pOne->m_bInvenDownload)",
      charName);
    return false;
  }

  player->SendMsg_InvenDownloadResult();
  return true;
}

bool CNetworkEX::CumDownloadRequest(unsigned int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bCumDownload)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: CumDownloadRequest()..  if(pOne->m_bCumDownload)",
      charName);
    return false;
  }

  player->SendMsg_CumDownloadResult();
  return true;
}

bool CNetworkEX::ForceDownloadRequest(unsigned int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bForceDownload)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ForceDownloadRequest()..  if(pOne->m_bForceDownload)",
      charName);
    return false;
  }

  player->SendMsg_ForceDownloadResult();
  return true;
}

bool CNetworkEX::QuestDownloadRequest(unsigned int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bQuestDownload)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: QuestDownloadRequest()..  if(pOne->m_bQuestDownload)",
      charName);
    return false;
  }

  player->SendMsg_QuestDownloadResult();
  player->SendMsg_QuestHistoryDownloadResult();
  return true;
}

bool CNetworkEX::SpecialDownloadRequest(unsigned int n, char * /*pBuf*/)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bSpecialDownload)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: SpecialDownloadRequest()..  if(pOne->m_bSpecialDownload)",
      charName);
    return false;
  }

  player->SendMsg_SpecialDownloadResult();
  return true;
}

bool CNetworkEX::OtherShapeRequest(unsigned int n, char *pBuf)
{
  CPlayer *dst = &g_Player[n];
  if (!dst->m_bOper)
  {
    return true;
  }

  const _other_shape_request_clzo *request =
    reinterpret_cast<const _other_shape_request_clzo *>(pBuf);
  if (request->byReqType == _other_shape_request_clzo::SHAPE_ALL
      || request->byReqType == _other_shape_request_clzo::SHAPE_PART)
  {
    if (request->wIndex < MAX_PLAYER)
    {
      CPlayer *src = &g_Player[request->wIndex];
      if (request->byReqType == _other_shape_request_clzo::SHAPE_PART)
      {
        src->SendMsg_OtherShapePart(dst);
        CPlayer::CashChangeStateFlag clientData(0);
        clientData.m_byStateFlag = request->byStateFlag;
        src->SendMsg_OtherShapePartEx_CashChange(dst, src->m_CashChangeStateFlag, clientData);
      }
      else
      {
        src->SendMsg_OtherShapeAll(dst);
      }
      return true;
    }

    const char *charName = dst->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: OtherShapeRequest()..  if(pRecv->wIndex >= MAX_PLAYER)",
      charName);
    return false;
  }

  const char *charName = dst->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: OtherShapeRequest()..  if(pRecv->byReqType != _other_shape_request_clzo::SHAPE_ALL && pRecv->byReqType !"
    "= _other_shape_request_clzo::SHAPE_PART)",
    charName);
  return false;
}

bool CNetworkEX::Revival(unsigned int n, char * /*pBuf*/)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_Revival(true);
  }
  return true;
}

bool CNetworkEX::ExitWorldRequest(unsigned int n, char * /*pBuf*/)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  if (m_bUseFG)
  {
    _socket *socket = GetSocket(0, n);
    _CcrFG_rs_CloseUserContext(&socket->m_hFGContext);
  }

  player->pc_ExitWorldRequest();
  return true;
}

bool CNetworkEX::LinkBoardDownloadRequest(unsigned int n, char * /*pBuf*/)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bLoad)
  {
    player->pc_LinkBoardRequest();
  }
  return true;
}

bool CNetworkEX::MacroDownLoadRequest(unsigned int n, char * /*pBuf*/)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bLoad)
  {
    player->SendMsg_MacroRequest();
  }
  return true;
}

bool CNetworkEX::AMP_DownloadRequest(unsigned int n, char * /*pBuf*/)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bAMPInvenDownload)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: AMP_InvenDownloadRequest()..  if(pOne->m_bAMPInvenDownload)",
      charName);
    return false;
  }

  player->SendMsg_AMPInvenDownloadResult();
  return true;
}

bool CNetworkEX::NextPoint(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_move_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return true;
  }

  DeCrypt_Move(
    reinterpret_cast<char *>(request),
    static_cast<int>(sizeof(*request)),
    static_cast<unsigned __int8>(player->m_byPlusKey + 1),
    player->m_wXorKey + 1);
  const unsigned int recvMoveCount = request->dwSerial;
  if (player->m_dwMoveCount < recvMoveCount)
  {
    if (recvMoveCount - player->m_dwMoveCount > 100u)
    {
      const char *charName = player->m_Param.GetCharNameA();
      g_Main.m_logMove.Write(
        "move count hack(start) warning! >> %s (one: %d, recv: %d)",
        charName,
        player->m_dwMoveCount,
        recvMoveCount);
    }
    player->m_dwMoveCount = recvMoveCount;

    const unsigned __int8 moveType = static_cast<unsigned __int8>(request->byMoveType);
    if (moveType == 0 || moveType == 1 || moveType == 2)
    {
      float targetPos[3]{};
      targetPos[0] = static_cast<float>(request->zTar[0]);
      targetPos[1] = request->fCur[1];
      targetPos[2] = static_cast<float>(request->zTar[1]);
      player->pc_MoveNext(moveType, request->fCur, targetPos, static_cast<unsigned __int8>(request->byDirect));
      return true;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: NextPoint()..  if(pRecv->byMoveType != move_type_walk && pRecv->byMoveType != move_type_run && pRecv->byMoveType != move_type_fly)",
      charName);
    return false;
  }

  const char *charName = player->m_Param.GetCharNameA();
  g_Main.m_logMove.Write(
    "move count hack(start) >> %s (one: %d, recv: %d)",
    charName,
    player->m_dwMoveCount,
    recvMoveCount);
  return false;
}

bool CNetworkEX::RealMovPosRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_real_movpos_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return true;
  }

  DeCrypt_Move(
    reinterpret_cast<char *>(request),
    static_cast<int>(sizeof(*request)),
    static_cast<unsigned __int8>(player->m_byPlusKey + 1),
    player->m_wXorKey + 2);
  const unsigned int recvMoveCount = request->dwSerial;
  if (player->m_dwMoveCount < recvMoveCount)
  {
    if (recvMoveCount - player->m_dwMoveCount > 100u)
    {
      const char *charName = player->m_Param.GetCharNameA();
      g_Main.m_logMove.Write(
        "move count hack(real) warning! >> %s (one: %d, recv: %d)",
        charName,
        player->m_dwMoveCount,
        recvMoveCount);
    }
    player->m_dwMoveCount = recvMoveCount;
    player->pc_RealMovPos(request->fCur);
    return true;
  }

  const char *charName = player->m_Param.GetCharNameA();
  g_Main.m_logMove.Write(
    "move count hack(real) >> %s (one: %d, recv: %d)",
    charName,
    player->m_dwMoveCount,
    recvMoveCount);
  return false;
}

bool CNetworkEX::Stop(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_player_stop_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return true;
  }

  DeCrypt_Move(
    reinterpret_cast<char *>(request),
    static_cast<int>(sizeof(*request)),
    static_cast<unsigned __int8>(player->m_byPlusKey + 1),
    player->m_wXorKey + 19);
  const unsigned int recvMoveCount = request->dwSerial;
  if (player->m_dwMoveCount < recvMoveCount)
  {
    if (recvMoveCount - player->m_dwMoveCount > 100u)
    {
      const char *charName = player->m_Param.GetCharNameA();
      g_Main.m_logMove.Write(
        "move count hack(stop) warning! >> %s (one: %d, recv: %d)",
        charName,
        player->m_dwMoveCount,
        recvMoveCount);
    }
    player->m_dwMoveCount = recvMoveCount;
    player->pc_MoveStop(request->fCur);
    return true;
  }

  const char *charName = player->m_Param.GetCharNameA();
  g_Main.m_logMove.Write(
    "move count hack(stop) >> %s (one: %d, recv: %d)",
    charName,
    player->m_dwMoveCount,
    recvMoveCount);
  return false;
}

bool CNetworkEX::GotoBasePortalRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_goto_base_portal_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  player->pc_GotoBasePortalRequest(request->wItemSerial);
  return true;
}

bool CNetworkEX::GotoAvatorRequest(unsigned int n, const _goto_avator_request_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  char nameBuffer[17]{};
  memcpy_0(nameBuffer, request->wszAvatorName, sizeof(request->wszAvatorName));
  nameBuffer[16] = '\0';
  if (strlen_0(nameBuffer) <= 16)
  {
    if (player->m_byUserDgr)
    {
      player->pc_GotoAvatorRequest(nameBuffer);
      return true;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: GotoAvatorRequest()..  if(pOne->m_byUserDgr == USER_DEGREE_STD)",
      charName);
    return false;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: GotoAvatorRequest()..  if(strlen(pRecv->szAvatorName) > max_name_len)",
    charName);
  return false;
}

bool CNetworkEX::MovePortalRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_move_portal_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  player->pc_MovePortal(
    static_cast<int>(static_cast<unsigned __int8>(request->byPortalIndex)),
    request->wConsumeSerial);
  return true;
}

bool CNetworkEX::RegistBindRequest(unsigned int n, char *pBuf)
{
  const auto *request = reinterpret_cast<const _regist_bind_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int16 npcIndex = request->wNPCIndex;
  CItemStoreManager *storeManager = CItemStoreManager::Instance();
  const int recordNum = static_cast<int>(storeManager->m_tblItemStore.GetRecordNum());
  if (npcIndex < recordNum)
  {
    CItemStore *store = nullptr;
    const int mapSerial = player->m_pCurMap->GetMapCode();
    CMapItemStoreList *storeList = CItemStoreManager::Instance()->GetMapItemStoreListBySerial(mapSerial);
    if (storeList)
    {
      store = storeList->GetItemStoreFromRecIndex(npcIndex);
      if (store)
      {
        player->pc_RegistBind(store);
      }
      return true;
    }
    return true;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: RegistBindRequest()..  if(pRecv->wNPCIndex >= CItemStoreManager::Instance()->m_tblItemStore.GetRecordNum())",
    charName);
  return false;
}

bool CNetworkEX::EnterReturnGateRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_enter_return_gate_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    CReturnGateController::Instance()->Enter(request->wGateIndex, player);
  }
  return true;
}

bool CNetworkEX::MoveTypeChangeRequeset(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_move_type_change_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int8 moveType = static_cast<unsigned __int8>(request->byMoveType);
  if (moveType == 0 || moveType == 1 || moveType == 2)
  {
    player->pc_MoveModeChangeRequest(moveType);
    return true;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: MoveTypeChangeRequeset()..  if(pRecv->byMoveType != move_type_walk && pRecv->byMoveType != move_type_run)",
    charName);
  return false;
}

bool CNetworkEX::MoveInfoRequeset(unsigned int n, char *pBuf)
{
// this is not a stub
  return true;
}

bool CNetworkEX::PlayerInfoResult(unsigned int n, char *pBuf)
{
// this is not a stub
  return true;
}

bool CNetworkEX::SelectClassRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_select_class_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int16 selClassIndex = request->wSelClassIndex;
  const int recordNum = g_Main.m_tblClass.GetRecordNum();
  if (selClassIndex < recordNum)
  {
    const unsigned __int8 selectRewardItem = static_cast<unsigned __int8>(request->bySelectRewardItem);
    if (selectRewardItem == 0xFF || selectRewardItem < 9u)
    {
      player->pc_SelectClassRequest(selClassIndex, selectRewardItem);
      return true;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: SelectClassRequest()..  if(pRecv->bySelectRewardItem >= max_class_bns_item)",
      charName);
    return false;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: SelectClassRequest()..  if(pRecv->wSelClassIndex >= g_Main.m_tblClass.GetRecordNum())",
    charName);
  return false;
}

bool CNetworkEX::InitClassRequest(unsigned int n, const _init_class_request_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  unsigned __int8 resultCode = 0;
  if (request->byInitType)
  {
    if (request->byInitType == 2)
    {
      resultCode = g_Main.m_pRFEvent_ClassRefine->DoEvent(player);
    }
  }
  else
  {
    resultCode = player->pc_InitClassRequest();
  }

  _init_class_result_zocl result{};
  result.byRet = static_cast<char>(resultCode);
  unsigned __int8 type[2]{11, 25};
  g_Network.m_pProcess[0]->LoadSendMsg(
    player->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&result),
    static_cast<unsigned __int16>(sizeof(result)));
  return true;
}

bool CNetworkEX::InitClassCostRequest(unsigned int n, char * /*pBuf*/)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    _init_class_cost_result_zocl result{};
    result.m_dwCostGold = player->GetInitClassCost();
    unsigned __int8 type[2]{11, 27};
    g_Network.m_pProcess[0]->LoadSendMsg(
      player->m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&result),
      static_cast<unsigned __int16>(sizeof(result)));
  }
  return true;
}

bool CNetworkEX::CanSelectClassRequest(unsigned int n, char * /*pBuf*/)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    _can_select_class_result_zocl result{};
    unsigned __int8 isRealClassUp = 0;
    result.byRet = static_cast<char>(player->pc_CanSelectClassRequest(reinterpret_cast<bool *>(&isRealClassUp)));
    result.byIsRealClassUp = static_cast<char>(isRealClassUp);
    unsigned __int8 type[2]{11, 29};
    g_Network.m_pProcess[0]->LoadSendMsg(
      player->m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&result),
      static_cast<unsigned __int16>(sizeof(result)));
  }
  return true;
}

bool CNetworkEX::SelectPcBangRewardRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    auto *request = reinterpret_cast<_pc_room_reward_info_clzo *>(pBuf);
    const unsigned int rewardIndex = request->dwPcRoomIndex;
    auto *selectItemIndex = reinterpret_cast<unsigned __int8 *>(request->bySelect_ItemIndex);
    CPcBangFavor *pcBang = CPcBangFavor::Instance();
    pcBang->PcBangGiveItem(player, rewardIndex, selectItemIndex, 5);
  }
  return true;
}

bool CNetworkEX::BuyStoreRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_buy_store_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  CItemStoreManager *storeManager = CItemStoreManager::Instance();
  const unsigned int recordNum = storeManager->m_tblItemStore.GetRecordNum();
  const unsigned int storeIndex = request->dwStoreIndex;
  if (storeIndex >= recordNum)
  {
    return true;
  }

  const unsigned __int8 buyNum = request->byBuyNum;
  if (buyNum > 0x64u)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: BuyStoreRequest().. if(pRecv->byBuyNum > trade_item_max_num)",
      charName);
    return false;
  }
  if (!buyNum)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: BuyStoreRequest().. if(pRecv->byBuyNum == 0)", charName);
    return false;
  }

  CMapItemStoreList *storeList = nullptr;
  if (player->m_pCurMap->m_pMapSet->m_nMapType == 1)
  {
    if (player->m_Param.m_pGuild)
    {
      CGuildRoomSystem *guildRoom = CGuildRoomSystem::GetInstance();
      if (guildRoom->IsGuildRoomMemberIn(player->m_Param.m_pGuild->m_dwSerial,
            player->m_ObjID.m_wIndex,
            player->m_pUserDB->m_dwSerial))
      {
        CItemStoreManager *manager = CItemStoreManager::Instance();
        storeList = manager->GetInstanceStoreListBySerial(player->m_Param.m_pGuild->m_dwSerial);
      }
    }
  }
  else
  {
    unsigned __int8 mapCode = static_cast<unsigned __int8>(-1);
    if (request->bUseNPCLinkIntem)
    {
      const int raceCode = player->m_Param.GetRaceCode();
      if (raceCode == 1)
      {
        mapCode = 1;
      }
      else if (raceCode == 2)
      {
        mapCode = 3;
      }
      else
      {
        mapCode = 0;
      }
    }
    else
    {
      mapCode = static_cast<unsigned __int8>(player->m_pCurMap->GetMapCode());
    }

    CItemStoreManager *manager = CItemStoreManager::Instance();
    storeList = manager->GetMapItemStoreListBySerial(mapCode);
  }

  if (!storeList)
  {
    return true;
  }

  CItemStore *store = storeList->GetItemStoreFromRecIndex(storeIndex);
  if (!store)
  {
    return true;
  }

  for (int j = 0; j < buyNum; ++j)
  {
    const unsigned int goodSerial = request->OfferList[j].dwGoodSerial;
    if (goodSerial >= static_cast<unsigned int>(store->m_nStorageItemNum))
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: BuyStoreRequest().. if(pRecv->OfferList[i].dwGoodSerial >= pStore->m_nStorageItemNum)",
        charName);
      return false;
    }
    const unsigned __int8 amount = request->OfferList[j].byAmount;
    if (!amount || amount > 0x63u)
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: BuyStoreRequest().. if(pRecv->OfferList[i].byAmount < 1 || pRecv->OfferList[i].byAmount > 99)",
        charName);
      return false;
    }
    const unsigned __int8 storageCode = request->OfferList[j].byStorageCode;
    if (storageCode >= 8u)
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: BuyStoreRequest().. if(pRecv->OfferList[i].byStorageCode >= _STORAGE_POS::STORAGE_NUM)",
        charName);
      return false;
    }
  }

  player->pc_BuyItemStore(
    store,
    buyNum,
    request->OfferList,
    request->bUseNPCLinkIntem);
  return true;
}

bool CNetworkEX::SellStoreRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_sell_store_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  CItemStoreManager *storeManager = CItemStoreManager::Instance();
  const unsigned int recordNum = storeManager->m_tblItemStore.GetRecordNum();
  const unsigned int storeIndex = request->dwStoreIndex;
  if (storeIndex >= recordNum)
  {
    return true;
  }

  const unsigned __int8 sellNum = request->bySellNum;
  if (sellNum > 0x64u)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: SellStoreRequest() : pRecv->bySellNum(%d) >= trade_item_max_num(%d)",
      charName,
      sellNum,
      100);
    return false;
  }
  if (!sellNum)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: SellStoreRequest() : if(pRecv->bySellNum == 0)", charName);
    return false;
  }

  for (int j = 0; j < sellNum; ++j)
  {
    const unsigned __int8 amount = request->Item[j].byAmount;
    if (!amount || amount > 0x63u)
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: SellStoreRequest() : pRecv->Item[i].byAmount (%d)",
        charName,
        amount);
      return false;
    }
    const unsigned __int8 storageCode = request->Item[j].byStorageCode;
    if (storageCode >= 8u)
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: SellStoreRequest() : pRecv->Item[i].byStorageCode (%d)",
        charName,
        storageCode);
      return false;
    }
  }

  unsigned __int8 mapCode = static_cast<unsigned __int8>(-1);
  if (request->bUseNPCLinkIntem)
  {
    const int raceCode = player->m_Param.GetRaceCode();
    if (raceCode == 1)
    {
      mapCode = 1;
    }
    else if (raceCode == 2)
    {
      mapCode = 3;
    }
    else
    {
      mapCode = 0;
    }
  }
  else
  {
    mapCode = static_cast<unsigned __int8>(player->m_pCurMap->GetMapCode());
  }

  CItemStoreManager *manager = CItemStoreManager::Instance();
  CMapItemStoreList *storeList = manager->GetMapItemStoreListBySerial(mapCode);
  if (!storeList)
  {
    return true;
  }

  CItemStore *store = storeList->GetItemStoreFromRecIndex(storeIndex);
  if (store)
  {
    player->pc_SellItemStore(
      store,
      sellNum,
      request->Item,
      request->bUseNPCLinkIntem);
  }
  return true;
}

bool CNetworkEX::StoreListRequest(unsigned int n, char * /*pBuf*/)
{
  CPlayer *player = &g_Player[n];
  player->SendMsg_StoreListResult();
  return true;
}

bool CNetworkEX::ExchangeDalantForGoldRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_exchange_dalant_for_gold_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }
  player->pc_ExchangeDalantForGold(request->dwDalant);
  return true;
}

bool CNetworkEX::ExchangeGoldForDalantRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_exchange_gold_for_dalant_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }
  player->pc_ExchangeGoldForDalant(request->dwGold);
  return true;
}

bool CNetworkEX::LimitItemNumRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_limit_item_num_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }
  player->pc_LimitItemNumRequest(request->dwStoreIndex);
  return true;
}

bool CNetworkEX::TalikRecorverList(unsigned int n, char * /*pBuf*/)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }
  player->m_kPvpCashPoint.SendMsg_TalikList(n);
  return true;
}

bool CNetworkEX::PvpCashRecorverWithTalik(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_pvp_cash_point_recorver_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }
  player->pc_PvpCashRecorver(request->dwItemSerial, static_cast<unsigned __int8>(request->byItemCnt));
  return true;
}

bool CNetworkEX::PcBangPrimiumCouponRequest(unsigned int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }
  player->m_kPcBangCoupon.ReceivePrimiumCoupon(n);
  return true;
}

bool CNetworkEX::AttackPersonalRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_attack_gen_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *target = reinterpret_cast<CCharacter *>(
    g_Main.GetObjectA(0, static_cast<unsigned __int8>(request->byID), request->wIndex));
  if (target)
  {
    if (static_cast<unsigned __int8>(request->byAttPart) < 5u)
    {
      player->pc_PlayAttack_Gen(
        target,
        static_cast<unsigned __int8>(request->byAttPart),
        request->wBulletSerial,
        request->wEffBulletSerial,
        false);
      return true;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: AttackPersonalRequest()..  if(pRecv->byAttPart >= base_fix_num)",
      charName);
    return false;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write("odd.. %s: AttackPersonalRequest()..  if(!pDst)", charName);
  return false;
}

bool CNetworkEX::AttackSkillRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_attack_skill_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *target = reinterpret_cast<CCharacter *>(
    g_Main.GetObjectA(0, static_cast<unsigned __int8>(request->byID), request->wIndex));
  const unsigned __int8 effectCode = static_cast<unsigned __int8>(request->byEffectCode);
  const bool effectCodeOutOfRange = effectCode > 4u;
  if (!effectCodeOutOfRange || effectCode == 2 || effectCode == 3)
  {
    if (g_Main.m_tblEffectData[effectCode].GetRecord(request->wSkillIndex))
    {
      float attackPos[3]{};
      attackPos[0] = static_cast<float>(request->zAreaPos[0]);
      attackPos[1] = 0.0f;
      attackPos[2] = static_cast<float>(request->zAreaPos[1]);
      player->pc_PlayAttack_Skill(
        target,
        attackPos,
        effectCode,
        request->wSkillIndex,
        request->wBulletSerial,
        request->wConsumeItemSerial,
        request->wEffBulletSerial);
      return true;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: AttackSkillRequest()..  if(!g_Main.m_tblEffectData[pRecv->byEffectCode].GetRecord(pRecv->wSkillIndex))",
      charName);
    return false;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: AttackSkillRequest()..  if(pRecv->byEffectCode != effect_code_skill && pRecv->byEffectCode != effect_code_class)",
    charName);
  return false;
}

bool CNetworkEX::AttackForceRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_attack_force_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *target = reinterpret_cast<CCharacter *>(
    g_Main.GetObjectA(0, static_cast<unsigned __int8>(request->byID), request->wIndex));
  if (target
      || (static_cast<unsigned __int8>(request->byID) == 0xFF && request->wIndex == 0xFFFF))
  {
    float areaPos[3]{};
    areaPos[0] = static_cast<float>(request->zAreaPos[0]);
    areaPos[1] = static_cast<float>(request->zAreaPos[1]);
    areaPos[2] = 0.0f;
    player->pc_PlayAttack_Force(
      target,
      areaPos,
      request->wForceSerial,
      request->wConsumeItemSerial,
      request->wEffBulletSerial);
    return true;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: AttackForceRequest()..  if(pRecv->byID != 0xFF || pRecv->wIndex != 0xFFFF)",
    charName);
  return false;
}

bool CNetworkEX::AttackUnitRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_attack_unit_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *target = reinterpret_cast<CCharacter *>(
    g_Main.GetObjectA(0, static_cast<unsigned __int8>(request->byID), request->wIndex));
  if (target)
  {
    if (static_cast<unsigned __int8>(request->byWeaponPart) < 2u)
    {
      player->pc_PlayAttack_Unit(target, static_cast<unsigned __int8>(request->byWeaponPart));
      return true;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: AttackUnitRequest()..  if(pRecv->byWeaponPart >= UNIT_BULLET_NUM)",
      charName);
    return false;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write("odd.. %s: AttackUnitRequest()..  if(!pDst)", charName);
  return false;
}

bool CNetworkEX::AttackTestRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_attack_test_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int8 effectCode = static_cast<unsigned __int8>(request->byEffectCode);
  if (effectCode == 0xFF || effectCode < 4u)
  {
    if (effectCode >= 4u
        || g_Main.m_tblEffectData[effectCode].GetRecord(static_cast<unsigned __int8>(request->byEffectIndex)))
    {
      const unsigned __int8 weaponPart = static_cast<unsigned __int8>(request->byWeaponPart);
      if (weaponPart == 0xFF || weaponPart < 2u)
      {
        return true;
      }

      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: AttackTestRequest()..  if(pRecv->byWeaponPart >= UNIT_BULLET_NUM)",
        charName);
      return true;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: AttackTestRequest()..  if(!g_Main.m_tblEffectData[pRecv->byEffectCode].GetRecord(pRecv->byEffectIndex))",
      charName);
    return true;
  }

  const int effectCodeValue = static_cast<unsigned __int8>(request->byEffectCode);
  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: AttackTestRequest()..  if(pRecv->byEffectCode != 0xFF && pRecv->byEffectCode >= EFFECT_CODE_NUM) : %d",
    charName,
    effectCodeValue);
  return true;
}

bool CNetworkEX::AttackSiegeRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_attack_siege_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (static_cast<unsigned __int8>(request->byAttPart) < 5u)
  {
    if (!player->IsActingSiegeMode())
    {
      auto *target = reinterpret_cast<CCharacter *>(
        g_Main.GetObjectA(0, static_cast<unsigned __int8>(request->byID), request->wIndex));
      float attackPos[3]{};
      attackPos[0] = static_cast<float>(request->zAttackPos[0]);
      attackPos[1] = static_cast<float>(request->zAttackPos[1]);
      attackPos[2] = 0.0f;
      player->pc_PlayAttack_Siege(
        target,
        attackPos,
        static_cast<unsigned __int8>(request->byAttPart),
        request->wBulletSerial,
        request->wEffBulletSerial);
    }
    return true;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: AttackSiegeRequest()..  if(pRecv->byAttPart >= base_fix_num)",
    charName);
  return false;
}

bool CNetworkEX::ItemboxTakeRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_itembox_take_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int16 boxIndex = request->wItemBoxIndex;
  if (boxIndex < MAX_ITEMBOX)
  {
    if (!player->m_EP.GetEff_State(26))
    {
      player->pc_TakeGroundingItem(&g_ItemBox[boxIndex], request->wAddSerial);
    }
    return true;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ItemboxTakeRequest()..  if(pRecv->wItemBoxIndex >= MAX_ITEMBOX)",
    charName);
  return false;
}

bool CNetworkEX::ThrowStorageRequest(unsigned int n, _throw_storage_request_clzo *request)
{
  CPlayer *player = &g_Player[n];
  auto *item = &request->Item;
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (!item->byStorageCode || item->byStorageCode == 1 || item->byStorageCode == 2)
  {
    if (!player->m_EP.GetEff_State(26))
    {
      player->pc_ThrowStorageItem(item);
    }
    return true;
  }

  const int storageCode = item->byStorageCode;
  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ThrowStorageRequest() : pRecv->Item.byStorageCode(%d)",
    charName,
    storageCode);
  return false;
}

bool CNetworkEX::UsePotionRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_use_potion_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return true;
  }

  if (request->Item.byStorageCode)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: UsePotionRequest()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  if (request->byReqType)
  {
    return true;
  }

  auto *targetPlayer = reinterpret_cast<CPlayer *>(
    g_Main.GetObjectA(0, 0, request->wTargetIndex));
  if (targetPlayer)
  {
    if (targetPlayer->m_bOper)
    {
      player->pc_UsePotionItem(targetPlayer, &request->Item);
    }
  }
  return true;
}

bool CNetworkEX::EquipPartRequest(unsigned int n, _equip_part_request_clzo *request)
{
  auto *player = &g_Player[n];
  auto *item = &request->Item;
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (item->byStorageCode)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: EquipPartRequest()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  player->pc_EquipPart(item);
  return true;
}

bool CNetworkEX::EmbellishRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_embellish_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->Item.byStorageCode)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: EmbellishRequest()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  player->pc_EmbellishPart(&request->Item, request->wChangeSerial);
  return true;
}

bool CNetworkEX::OffPartRequest(unsigned int n, _off_part_request_clzo *request)
{
  auto *player = &g_Player[n];
  auto *item = &request->Item;
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (item->byStorageCode == 1 || item->byStorageCode == 2)
  {
    player->pc_OffPart(item);
    return true;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: OffPartRequest()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::EQUIP && pRecv->Item.byStorageCode != _STORAGE_POS::EMBELLISH)",
    charName);
  return false;
}

bool CNetworkEX::MakeItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_make_item_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->byMaterialNum <= 0x64u)
  {
    const int manualIndex = request->wManualIndex;
    const int recordNum = g_Main.m_tblItemMakeData.GetRecordNum();
    if (manualIndex < recordNum)
    {
      player->pc_MakeItem(
        &request->ipMakeTool,
        request->wManualIndex,
        request->byMaterialNum,
        request->ipMaterials);
      return true;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: MakeItemRequest()..  if(pRecv->wManualIndex >= g_Main.m_tblItemMakeData.GetRecordNum())",
      charName);
    return false;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: MakeItemRequest()..  if(pRecv->byMaterialNum > _make_item_request_clzo::material_num)",
    charName);
  return false;
}

bool CNetworkEX::UpgradeItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_upgrade_item_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int8 jewelNum = request->byJewelNum;
  if (jewelNum <= 4u)
  {
    if (request->m_posTalik.byStorageCode)
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: UpgradeItemRequest()..  if(pRecv->m_posTalik.byStorageCode != _STORAGE_POS::INVEN)",
        charName);
      return false;
    }

    for (int j = 0; j < jewelNum; ++j)
    {
      if (request->m_posUpgJewel[j].byStorageCode)
      {
        const char *charName = player->m_Param.GetCharNameA();
        m_LogFile.Write(
          "odd.. %s: UpgradeItemRequest()..  if(pRecv->m_posUpgJewel[i].byStorageCode != _STORAGE_POS::INVEN)",
          charName);
        return false;
      }
    }

    if (request->m_posToolItem.byStorageCode)
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: UpgradeItemRequest()..  if(pRecv->m_posToolItem.byStorageCode != _STORAGE_POS::INVEN)",
        charName);
      return false;
    }

    if (!request->m_posUpgItem.byStorageCode || request->m_posUpgItem.byStorageCode == 1)
    {
      player->pc_UpgradeItem(
        &request->m_posTalik,
        &request->m_posToolItem,
        &request->m_posUpgItem,
        jewelNum,
        request->m_posUpgJewel);
      return true;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: UpgradeItemRequest()..  if(pRecv->m_posToolItem.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: UpgradeItemRequest()..  if(pRecv->byJewelNum > upgrade_jewel_num)",
    charName);
  return false;
}

bool CNetworkEX::DownGradeItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_downgrade_item_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->m_posTalik.byStorageCode)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: DownGradeItemRequest()..  if(pRecv->m_posTalik.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  if (request->m_posToolItem.byStorageCode)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: DownGradeItemRequest()..  if(pRecv->m_posToolItem.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  if (!request->m_posUpgItem.byStorageCode || request->m_posUpgItem.byStorageCode == 1)
  {
    player->pc_DowngradeItem(&request->m_posTalik, &request->m_posToolItem, &request->m_posUpgItem);
    return true;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: DownGradeItemRequest()..  if(pRecv->m_posUpgItem.byStorageCode != _STORAGE_POS::INVEN && pRecv->m_posUpgItem.byStorageCode != _STORAGE_POS::EQUIP)",
    charName);
  return false;
}

bool CNetworkEX::AddBagRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_add_bag_request_clzo *>(pBuf);
  auto *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  player->pc_AddBag(request->wBagSerial);
  return true;
}

bool CNetworkEX::UseRecoverLossExpItemRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_use_recover_loss_exp_item_request_clzo *>(pBuf);
  auto *player = &g_Player[n];
  if (player->m_bOper)
  {
    _use_recover_lossexp_item_result_zocl result{};
    result.cRet = player->pc_UseRecoverLossExpItem(request->wItemSerial);
    unsigned __int8 type[2]{7, 27};
    g_Network.m_pProcess[0]->LoadSendMsg(
      player->m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&result),
      static_cast<unsigned __int16>(sizeof(result)));
  }
  return true;
}

bool CNetworkEX::CombineItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_combine_item_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->byMaterialNum <= 0x64u)
  {
    const int manualIndex = request->wManualIndex;
    const int recordNum = g_Main.m_tblItemCombineData.GetRecordNum();
    if (manualIndex < recordNum)
    {
      player->pc_CombineItem(
        request->wManualIndex,
        request->byMaterialNum,
        request->ipMaterials,
        request->wOverlapItemSerial);
      return true;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: CombineItemRequest()..  if(pRecv->wManualIndex >= g_Main.m_tblItemCombineData.GetRecordNum())",
      charName);
    return false;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: CombineItemRequest()..  if(pRecv->byMaterialNum > _combine_item_request_clzo::material_num)",
    charName);
  return false;
}

bool CNetworkEX::ExchangeItemRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_exchange_item_request_clzo *>(pBuf);
  auto *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const int manualIndex = request->wManualIndex;
  const int recordNum = g_Main.m_tblItemExchangeData.GetRecordNum();
  if (manualIndex < recordNum)
  {
    player->pc_ExchangeItem(request->wManualIndex, request->wTarSerial);
    return true;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ExchangeItemRequest()..  if(pRecv->wManualIndex >= g_Main.m_tblItemExchangeData.GetRecordNum())",
    charName);
  return false;
}

bool CNetworkEX::CombineExItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_combine_ex_item_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->byCombineSlotNum <= 5u)
  {
    const unsigned int recordNum = ItemCombineMgr::ms_tbl_ItemCombine.GetRecordNum();
    if (request->wManualIndex < recordNum)
    {
      player->pc_CombineItemEx(request);
      return true;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: CombineItemRequest()..  if(pRecv->wManualIndex >= ItemCombineMgr::ms_tbl_ItemCombine[eCOMMON].GetRecordNum()))",
      charName);
    return false;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: CombineExItemRequest()..  if(pRecv->byCombineSlotNum > _combine_ex_item_request_clzo::combineslot_max ))",
    charName);
  return false;
}

bool CNetworkEX::CombineExItemAcceptRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_combine_ex_item_accept_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->SelectItemBuff.bySelectNum < 0x18u)
  {
    if (request->byDlgType == 0xFF || request->byDlgType == 1 || !request->byDlgType)
    {
      player->pc_CombineItemExAccept(request);
      return true;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: CombineExItemAcceptRequest().. \tIsNotDlgType  ", charName);
    return false;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: CombineExItemAcceptRequest().. \tif(\tpRecv->SelectItemBuff.bySelectNum >= _combine_ex_item_accept_request_clzo::eMaxSelectItemNum ) ",
    charName);
  return false;
}

bool CNetworkEX::UseFireCrackerItemRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_use_firecracker_item_request_clzo *>(pBuf);
  auto *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const int result = player->pc_UseFireCracker(request->wItemSerial);
  const int errorCode = (result < 0) ? result : 0;

  _use_fire_cracker_item_result_zocl sendMsg{};
  sendMsg.cRet = static_cast<char>(errorCode);
  sendMsg.wSerial = request->wItemSerial;
  unsigned __int8 type[2]{7, 38};
  g_Network.m_pProcess[0]->LoadSendMsg(
    player->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&sendMsg),
    static_cast<unsigned __int16>(sizeof(sendMsg)));

  if (result >= 0)
  {
    _use_fire_cracker_item_inform_zocl circleMsg{};
    circleMsg.dwUserObjSerial = player->m_dwObjSerial;
    circleMsg.wItemIndex = static_cast<unsigned __int16>(result);
    unsigned __int8 circleType[2]{7, 39};
    player->CircleReport(
      circleType,
      reinterpret_cast<char *>(&circleMsg),
      static_cast<int>(sizeof(circleMsg)),
      0,
      false);
  }
  return true;
}

bool CNetworkEX::SetItemCheckRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_set_item_check_request_clzo *>(pBuf);
  auto *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_SetItemCheckRequest(
      request->dwSetIndex,
      static_cast<unsigned __int8>(request->bySetItemNum),
      static_cast<unsigned __int8>(request->bySetEffectNum),
      request->bSet);
  }
  return true;
}

bool CNetworkEX::UseSoccerBallItemRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_use_soccer_ball_item_request_clzo *>(pBuf);
  auto *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  unsigned __int16 itemIndex[16]{};
  itemIndex[0] = 0xFFFFu;
  unsigned __int8 result = player->pc_UserSoccerBall(request->wItemSerial, itemIndex);

  _use_soccer_ball_item_result_zocl sendMsg{};
  sendMsg.byRet = static_cast<char>(result);
  sendMsg.wSerial = request->wItemSerial;
  unsigned __int8 type[2]{7, 47};
  g_Network.m_pProcess[0]->LoadSendMsg(
    player->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&sendMsg),
    static_cast<unsigned __int16>(sizeof(sendMsg)));

  if (!result)
  {
    _use_soccer_ball_item_inform_zocl circleMsg{};
    circleMsg.dwUserObjSerial = player->m_dwObjSerial;
    circleMsg.wItemIndex = itemIndex[0];
    circleMsg.bTakeSoccerBall = player->m_bTakeSoccerBall;
    unsigned __int8 circleType[2]{7, 48};
    player->CircleReport(
      circleType,
      reinterpret_cast<char *>(&circleMsg),
      static_cast<int>(sizeof(circleMsg)),
      0,
      false);
    player->SenseState();
  }
  return true;
}

bool CNetworkEX::UseRadarItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_use_radar_item_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->Item.byStorageCode)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: UseRadarRequest()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  player->pc_UseRadarItem(&request->Item, request->wConsumeItemSerial);
  return true;
}

bool CNetworkEX::RadarCharListRequest(unsigned int n, char *pBuf)
{
auto *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_RadarCharInfo();
  }
  return true;
}

bool CNetworkEX::NPCLinkCheckItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *item = reinterpret_cast<_STORAGE_POS_INDIV *>(pBuf);
  if (player->m_bOper)
  {
    player->pc_NPCLinkCheckItemRequest(item);
  }
  return true;
}

bool CNetworkEX::UseRecallTeleportItemRequest(unsigned int n, _use_recall_teleport_item_request_clzo *request)
{
  auto *player = &g_Player[n];
  auto *item = &request->Item;
  if (!player->m_bOper)
  {
    return true;
  }

  if (item->byStorageCode)
  {
    player->SendMsg_UsePotionResult(0x19u, item->wItemSerial, 0);
  }

  char targetName[17]{};
  strncpy_s(targetName, sizeof(targetName), request->wszRecallName, 0x10u);
  CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
  if (manager->IsNormalString(targetName))
  {
    CPlayer *targetPlayer = GetPtrPlayerFromName(g_Player, MAX_PLAYER, targetName);
    if (targetPlayer)
    {
      player->pc_UsePotionItem(targetPlayer, item);
    }
    else
    {
      CRecallEffectController *recall = CRecallEffectController::Instance();
      recall->SendRecallReqeustResult(0x11u, player);
    }
    return true;
  }

  CRecallEffectController *recall = CRecallEffectController::Instance();
  recall->SendRecallReqeustResult(0x11u, player);
  return true;
}

bool CNetworkEX::CharacterRenameCash(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_character_rename_cash_request_clzo *>(pBuf);
  auto *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->Item.byStorageCode)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: CharacterRenameCash()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  char newName[17]{};
  strncpy_s(newName, sizeof(newName), request->szNewName, 0x10u);
  player->pc_CharacterRenameCash(
    static_cast<bool>(request->bChange),
    &request->Item,
    newName);
  return true;
}

bool CNetworkEX::TalikCrystalExchangeRequest(unsigned int n, _MSG_HEADER *pHeader, char *pBuf)
{
auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_talik_crystal_exchange_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int8 exchangeNum = static_cast<unsigned __int8>(request->byExchangeNum);
  if (exchangeNum <= 0x18u)
  {
    if (exchangeNum)
    {
      player->pc_TalikCrystalExchange(exchangeNum, request->Item);
      return true;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: TalikCrystalExchangeRequest() : if(pRecv->byExchangeNum == 0)",
      charName);
    return false;
  }

  const int exchangeNumValue = exchangeNum;
  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: TalikCrystalExchangeRequest() : pRecv->byExchangeNum(%d) >= _talik_crystal_exchange_clzo::exchange_zone(%d)",
    charName,
    exchangeNumValue,
    24);
  return false;
}

bool CNetworkEX::EnterWorldRequest(unsigned int n, _MSG_HEADER *pMsgHeader, char *pBuf)
{
  CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
  if (!manager->CheckEnterWorldRequest(n, pBuf))
  {
    return false;
  }

  unsigned __int8 resultCode = 0;
  if (static_cast<int>(pMsgHeader->m_wSize) - 62 != 0)
  {
    m_LogFile.Write(
      "KeyCheckError.. Socket( %d ): ( pMsgHeader->m_wSize - sizeof(_MSG_HEADER) ) - sizeof( _enter_world_request_zone ) != 0 ",
      n);
    resultCode = static_cast<unsigned __int8>(-14);
  }
  else
  {
    const _enter_world_request_zone *request = reinterpret_cast<_enter_world_request_zone *>(pBuf);
    if (strlen_0(request->szClientVerCheckKey) == 32)
    {
      if (strcmp_0(CMainThread::ms_szClientVerCheck, "X")
          && strncmp(CMainThread::ms_szClientVerCheck, request->szClientVerCheckKey, 0x20u))
      {
        m_LogFile.Write(
          "KeyCheckError.. Socket( %d ): EnterWorldRequest().. if( strncmp( CMainThread::ms_szClientVerCheck, pRecv->szClientVerCheckKey, CMainThread::eClinetCheckMax ) )",
          n);
        resultCode = static_cast<unsigned __int8>(-14);
      }
    }
    else
    {
      m_LogFile.Write(
        "KeyCheckError.. Socket( %d ): EnterWorldRequest().. if( strlen(pRecv->szClientVerCheckKey) != CMainThread::eClinetCheckMax )",
        n);
      resultCode = static_cast<unsigned __int8>(-14);
    }
  }

  if (resultCode)
  {
    _enter_world_result_zone result{};
    result.byResult = resultCode;
    result.byUserGrade = 0;
    result.bySvrType = g_Main.m_byWorldType;
    unsigned __int8 type[2]{1, 2};
    const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
    g_Network.m_pProcess[0]->LoadSendMsg(
      g_UserDB[n].m_idWorld.wIndex,
      type,
      reinterpret_cast<char *>(&result),
      len);
    Close(0, n, true, nullptr);
    return true;
  }

  _socket *socket = GetSocket(0, n);
  _enter_world_request_zone *request = reinterpret_cast<_enter_world_request_zone *>(pBuf);
  if (g_UserDB[n].Enter_Account(
        request->dwAccountSerial,
        socket->m_Addr.sin_addr.S_un.S_addr,
        request->dwProtocolVer,
        request->dwMasterKey))
  {
    socket->m_bEnterCheck = true;
    m_pProcess[0]->StartSpeedHackCheck(n, g_UserDB[n].m_szAccountID);
  }
  else
  {
    Close(0, n, false, nullptr);
  }
  return true;
}

bool CNetworkEX::MoveLobbyRequest(unsigned int n, char *pBuf)
{
if (!g_Player[n].m_bLive)
  {
    return true;
  }
  if (!g_UserDB[n].Lobby_Char_Request())
  {
    char buffer[144]{};
    sprintf(buffer, "CLOSE>> MoveLobbyRequest ID:%s", g_UserDB[n].m_szAccountID);
    Close(0, n, false, buffer);
  }
  return true;
}

bool CNetworkEX::RegedCharRequest(unsigned int n, char *pBuf)
{
if (!g_UserDB[n].m_bActive)
  {
    return true;
  }
  if (!g_UserDB[n].Reged_Char_Request())
  {
    char buffer[144]{};
    sprintf(buffer, "CLOSE>> RegedCharRequest ID:%s", g_UserDB[n].m_szAccountID);
    Close(0, n, false, buffer);
  }
  return true;
}

bool CNetworkEX::AddCharRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_add_char_request_clzo *>(pBuf);
  if (!g_UserDB[n].m_bActive)
  {
    return true;
  }

  char charName[17]{};
  memcpy_0(charName, request->wszCharName, 0x10u);
  charName[16] = '\0';

  if (strlen_0(charName) <= 0x10)
  {
    if (static_cast<unsigned __int8>(request->bySlotIndex) < 3u)
    {
      if (static_cast<unsigned __int8>(request->byRaceSexCode) < 5u)
      {
        if (strlen_0(request->wszClassCode) <= 4)
        {
          if (Major_Add_Character)
          {
            return false;
          }

          if (!g_UserDB[n].Insert_Char_Request(
                charName,
                static_cast<unsigned __int8>(request->bySlotIndex),
                static_cast<unsigned __int8>(request->byRaceSexCode),
                request->wszClassCode,
                request->dwMakeCharKey))
          {
            char buffer[144]{};
            sprintf(buffer, "CLOSE>> AddCharRequest ID:%s", g_UserDB[n].m_szAccountID);
            Close(0, n, false, buffer);
          }
          return true;
        }
        return false;
      }
      return false;
    }
    return false;
  }

  m_LogFile.Write(
    "odd.. id( %s ): AddCharRequest().. if(strlen(pRecv->szCharName) > max_name_len)",
    g_UserDB[n].m_szAccountID);
  return false;
}

bool CNetworkEX::DelCharRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_char_slot_request_clzo *>(pBuf);
  if (!g_UserDB[n].m_bActive)
  {
    return true;
  }
  const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->bySlotIndex);
  if (slotIndex < 3u)
  {
    if (!g_UserDB[n].Delete_Char_Request(slotIndex))
    {
      char buffer[144]{};
      sprintf(buffer, "CLOSE>> DelCharRequest ID:%s", g_UserDB[n].m_szAccountID);
      Close(0, n, false, buffer);
    }
    return true;
  }

  m_LogFile.Write(
    "odd.. id( %s ): DelCharRequest().. if(pRecv->bySlotIndex >= MAX_CHAR_PER_WORLDUSER)",
    g_UserDB[n].m_szAccountID);
  return false;
}

bool CNetworkEX::SelCharRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_char_slot_request_clzo *>(pBuf);
  if (!g_UserDB[n].m_bActive)
  {
    return true;
  }
  if (g_UserDB[n].m_byUserDgr || g_UserDB[n].m_byUILock == 2)
  {
    const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->bySlotIndex);
    if (slotIndex < 3u)
    {
      if (!g_UserDB[n].Select_Char_Request(slotIndex))
      {
        char buffer[144]{};
        sprintf(buffer, "CLOSE>> SelCharRequest ID:%s", g_UserDB[n].m_szAccountID);
        Close(0, n, false, buffer);
      }
      return true;
    }

    m_LogFile.Write(
      "odd.. id( %s ): SelCharRequest().. if(pRecv->bySlotIndex >= MAX_CHAR_PER_WORLDUSER)",
      g_UserDB[n].m_szAccountID);
    return false;
  }

  m_LogFile.Write(
    "odd.. id( %s ): SelCharRequest().. UI Lock Not Certified!",
    g_UserDB[n].m_szAccountID);
  return false;
}

bool CNetworkEX::TutorialProcessReport(int n, char *pBuf)
{
if (g_UserDB[n].m_bActive)
  {
    CUserDB::s_MgrLobbyHistory.tutorial_process_report_recv(g_UserDB[n].m_szLobbyHistoryFileName);
    g_UserDB[n].m_dwOperLobbyTime = GetLoopTime();
  }
  return true;
}

bool CNetworkEX::AliveCharRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_alive_char_request_clzo *>(pBuf);
  if (!g_UserDB[n].m_bActive)
  {
    return true;
  }

  char charName[17]{};
  memcpy_0(charName, request->wszNewName, 0x10u);
  charName[16] = '\0';

  if (strlen_0(charName) <= 0x10)
  {
    if (static_cast<unsigned __int8>(request->bySlotIndex) < 3u)
    {
      if (!request->byCase || request->byCase == 1)
      {
        g_UserDB[n].Alive_Char_Request(
          static_cast<unsigned __int8>(request->byCase),
          request->dwSerial,
          charName,
          static_cast<unsigned __int8>(request->bySlotIndex));
        return true;
      }

      m_LogFile.Write("odd.. AliveCharRequest() .. if(pRecv->byCase != 0 && pRecv->byCase != 1)");
      return false;
    }

    m_LogFile.Write("odd.. AliveCharRequest() .. if(pRecv->bySlotIndex >= MAX_CHAR_PER_WORLDUSER)");
    return false;
  }

  m_LogFile.Write(
    "odd.. id( %s ): AddCharRequest().. if(strlen(pRecv->szCharName) > max_name_len)",
    g_UserDB[n].m_szAccountID);
  return false;
}

bool CNetworkEX::NotifyLocalTimeRequest(int n, char *pBuf)
{
_notify_local_time_result_zocl result{};
  memset_0(&result, 0, sizeof(result));
  unsigned __int8 type[2]{1, 27};

  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    __int64 now{};
    time_5(&now);
    tm *local = localtime_3(&now);
    if (local)
    {
      result.tm_sec = local->tm_sec;
      result.tm_min = local->tm_min;
      result.tm_hour = local->tm_hour;
      result.tm_mday = local->tm_mday;
      result.tm_mon = local->tm_mon;
      result.tm_year = local->tm_year;
      result.tm_wday = local->tm_wday;
      result.tm_yday = local->tm_yday;
      result.tm_isdst = local->tm_isdst;
    }
    else
    {
      result.byRet = 2;
    }

    const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
    g_Network.m_pProcess[0]->LoadSendMsg(
      player->m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&result),
      len);
    return true;
  }

  result.byRet = 1;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(
    player->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&result),
    len);
  return true;
}

bool CNetworkEX::ChatOperatorRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_operator_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 raceCode = static_cast<unsigned __int8>(request->byRaceCode);
  if (raceCode < 3u || raceCode == 0xFF)
  {
    const unsigned __int8 messageSize = static_cast<unsigned __int8>(request->bySize);
    if (messageSize == 0xFF)
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: ChatOperatorRequest()..  if(pRecv->bySize > max_message_size)",
        charName);
    }
    else
    {
      char chatData[272]{};
      memcpy_0(chatData, request->wszChatData, messageSize);
      chatData[messageSize] = 0;
      player->pc_ChatOperatorRequest(raceCode, chatData);
    }
    return true;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ChatOperatorRequest()..  if(pRecv->byRaceCode >= RACE_NUM && pRecv->byRaceCode != 0xFF)",
    charName);
  return false;
}

bool CNetworkEX::ChatCircleRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }
  if (player->m_EP.GetEff_Have(50) > 0.0)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ChatCircleRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, request->wszChatData, size);
    chatData[size] = 0;
    player->pc_ChatCircleRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatFarRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_far_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }
  if (player->m_EP.GetEff_Have(50) > 0.0)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: ChatFarRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char name[56]{};
    char chatData[272]{};
    memcpy_0(name, request->wszName, 0x10u);
    name[16] = 0;
    memcpy_0(chatData, request->wszChatData, size);
    chatData[size] = 0;
    player->pc_ChatFarRequest(name, chatData);
  }
  return true;
}

bool CNetworkEX::ChatPartyRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }
  if (player->m_EP.GetEff_Have(50) > 0.0)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: ChatPartyRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, request->wszChatData, size);
    chatData[size] = 0;
    player->pc_ChatPartyRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatRaceRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }
  if (player->m_EP.GetEff_Have(50) > 0.0)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: ChatRaceRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, request->wszChatData, size);
    chatData[size] = 0;
    player->pc_ChatRaceRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatCheatRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: ChatCheatRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char command[272]{};
    memcpy_0(command, request->wszChatData, size);
    command[size] = 0;
    ProcessCheatCommand(player, command);
  }
  return true;
}

bool CNetworkEX::ChatManageRequest(unsigned int n, char *pBuf)
{
// this is not a stub
  return true;
}

bool CNetworkEX::ChatMgrWhisperRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ChatMgrWhisperRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, request->wszChatData, size);
    chatData[size] = 0;
    player->pc_ChatMgrWhisperRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatMapRecvYesOrNo(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_toggle_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bLoad)
  {
    player->m_bRecvMapChat = request->byRecvType == 0;
  }
  return true;
}

bool CNetworkEX::ChatMapRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: ChatMapRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, request->wszChatData, size);
    chatData[size] = 0;
    player->pc_ChatMapRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatRaceBossRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ChatRaceBossRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, request->wszChatData, size);
    chatData[size] = 0;
    player->pc_ChatRaceBossRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatGuildEstSenRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ChatGuildEstSenRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, request->wszChatData, size);
    chatData[size] = 0;
    player->pc_ChatGuildEstSenRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatRePresentationRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ChatRePresentationRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, request->wszChatData, size);
    chatData[size] = 0;
    player->pc_ChatRePresentationRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatAllRecvYesOrNo(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_toggle_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bLoad)
  {
    player->m_bRecvAllChat = request->byRecvType == 0;
  }
  return true;
}

bool CNetworkEX::ChatAllRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: ChatAllRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, request->wszChatData, size);
    chatData[size] = 0;
    player->pc_ChatAllRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatGreetingMsg_GM(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }
  if (player->m_pUserDB->m_byUserDgr < 2u)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ChatGreetingMsg_GM()..  if(pRecv->bySize > max_message_size)",
      charName);
    return true;
  }

  if (IsSQLValidString(request->wszChatData))
  {
    char msg[272]{};
    memcpy_0(msg, request->wszChatData, size);
    msg[size] = 0;
    char *charName = player->m_Param.GetCharNameW();
    g_Main.pc_SetMainGreetingMsg( charName, msg);
  }
  else
  {
    const char *charName = player->m_Param.GetCharNameA();
    g_Main.m_logSystemError.Write(
      "CNetworkEX::ChatGreetingMsg_GM() : %u(%s) ::IsSQLValidString( pRecv->wszChatData ) Invalid!",
      player->m_dwObjSerial,
      charName);
  }
  return true;
}

bool CNetworkEX::ChatGreetingMsg_RACE(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ChatGreetingMsg_RACE()..  if(pRecv->bySize > max_message_size)",
      charName);
    return true;
  }

  const unsigned int charSerial = player->m_Param.GetCharSerial();
  const int raceCode = player->m_Param.GetRaceCode();
  CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
  const unsigned int bossSerial = rank->GetCurrentRaceBossSerial(static_cast<unsigned __int8>(raceCode), 0);
  if (charSerial != bossSerial)
  {
    return true;
  }

  if (IsSQLValidString(request->wszChatData))
  {
    char msg[272]{};
    memcpy_0(msg, request->wszChatData, size);
    msg[size] = 0;
    char *bossName = player->m_Param.GetCharNameA();
    const int senderRace = player->m_Param.GetRaceCode();
    g_Main.pc_SetRaceGreetingMsg( senderRace, bossName, msg);
  }
  else
  {
    const char *charName = player->m_Param.GetCharNameA();
    g_Main.m_logSystemError.Write(
      "CNetworkEX::ChatGreetingMsg_RACE() : %u(%s) ::IsSQLValidString( pRecv->wszChatData ) Invalid!",
      player->m_dwObjSerial,
      charName);
  }
  return true;
}

bool CNetworkEX::ChatGreetingMsg_GUILD(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ChatGreetingMsg_GUILD()..  if(pRecv->bySize > max_message_size)",
      charName);
    return true;
  }

  if (player->m_Param.m_pGuild)
  {
    const unsigned int masterSerial = player->m_Param.m_pGuild->GetGuildMasterSerial();
    if (masterSerial != static_cast<unsigned int>(-1))
    {
      const unsigned int charSerial = player->m_Param.GetCharSerial();
      if (masterSerial == charSerial)
      {
        if (IsSQLValidString(request->wszChatData))
        {
          char msg[272]{};
          memcpy_0(msg, request->wszChatData, size);
          msg[size] = 0;
          if (player->m_Param.m_pGuild)
          {
            player->m_Param.m_pGuild->SetGreetingmsg_GUILD(msg);
          }
          return true;
        }

        const char *charName = player->m_Param.GetCharNameA();
        g_Main.m_logSystemError.Write(
          "CNetworkEX::ChatGreetingMsg_GUILD() : %u(%s) ::IsSQLValidString( pRecv->wszChatData ) Invalid!",
          player->m_dwObjSerial,
          charName);
        return true;
      }
    }
  }

  return true;
}

bool CNetworkEX::ChatTradeRequestMsg(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_trade_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 messageSize = static_cast<unsigned __int8>(request->bySize);
  if (messageSize == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: ChatTradeMsg()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char tradeMsg[272]{};
    memcpy_0(tradeMsg, request->wszChatData, messageSize);
    tradeMsg[messageSize] = 0;
    player->pc_ChatTradeRequestMsg(static_cast<unsigned __int8>(request->byRaceCode), tradeMsg);
  }
  return true;
}

bool CNetworkEX::ChatGuildRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_guild_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }
  if (player->m_EP.GetEff_Have(50) > 0.0)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: ChatGuildRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, request->wszChatData, size);
    chatData[size] = 0;
    player->pc_ChatGuildRequest(request->dwDstSerial, chatData);
  }
  return true;
}

bool CNetworkEX::ChatMultiFarRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_multi_far_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 transNum = request->byTransNum;
  const unsigned __int8 *src = reinterpret_cast<const unsigned __int8 *>(&request->byTransNum) + 1;

  if (transNum <= 4u)
  {
    _w_name dstNames[6]{};
    for (int j = 0; j < transNum; ++j)
    {
      unsigned __int8 nameLen = 0;
      memcpy_0(&nameLen, src, 1u);
      ++src;
      if (nameLen > 0x10u)
      {
        const char *charName = player->m_Param.GetCharNameA();
        m_LogFile.Write("odd.. %s: ChatMultiFarRequest()..  if(byNameLen > max_name_len)", charName);
        return true;
      }
      memcpy_0(dstNames[j].name, src, nameLen);
      dstNames[j].name[nameLen] = 0;
      src += nameLen;
    }

    unsigned __int8 msgSize = 0;
    memcpy_0(&msgSize, src, 1u);
    ++src;
    if (msgSize == 0xFF)
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write("odd.. %s: ChatMultiFarRequest()..  if(byMsgSize >= max_message_size)", charName);
    }
    else
    {
      char msg[264]{};
      memcpy_0(msg, src, msgSize);
      src += msgSize;
      msg[msgSize] = 0;
      player->pc_ChatMultiFarRequest(transNum, dstNames, msg);
    }
    return true;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write("odd.. %s: ChatMultiFarRequest()..  if(byTransNum > max_multi_far_msg)", charName);
  return true;
}

bool CNetworkEX::ChatRaceBossCryRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ChatRaceBossCryRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, request->wszChatData, size);
    chatData[size] = 0;
    player->pc_ChatRaceBossCryRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatGmNoticeRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_chat_message_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(request->bySize);
  if (size == 0xFF)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ChatGmNoticeRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, request->wszChatData, size);
    chatData[size] = 0;
    player->pc_ChatGmNoticeRequest(chatData);
  }
  return true;
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
        return OpenWorldSuccessResult(n, reinterpret_cast<const _open_world_success_acwr *>(pMsg));
      case 4:
        return true;
      case 5:
        return ForceCloseCommand(n, reinterpret_cast<const _force_close_command_acwr *>(pMsg));
      case 6:
        return TransAccountInform(n, reinterpret_cast<const _trans_account_inform_acwr *>(pMsg));
      case 9:
        return EnterWorldResult(n, reinterpret_cast<const _enter_world_result_acwr *>(pMsg));
      case 14:
        return UILockInitResult(n, reinterpret_cast<const _uilock_init_result_acwr *>(pMsg));
      case 16:
        return UILockUpdateResult(n, reinterpret_cast<const _uilock_update_result_acwr *>(pMsg));
      case 18:
        return UILockRefreshResult(n, reinterpret_cast<const _uilock_user_refresh_info_result_acwr *>(pMsg));
      case 19:
        return CheckIsBlockIPResult(n, reinterpret_cast<const _check_is_block_ip_result_acwr *>(pMsg));
      case 20:
        return OpenWorldFailureResult(n, reinterpret_cast<const _open_world_failure_acwr *>(pMsg));
      case 21:
        return ConEventTotalSalesCheck(static_cast<int>(n));
      case 22:
        return DisconnectGuildWarCharacterRequest(
          static_cast<int>(n),
          reinterpret_cast<const _disconnect_guild_war_character_request_acwr *>(pMsg));
      case 24:
        return ManageClientLimitRunRequest(reinterpret_cast<const _manage_client_limit_run_request_acwr *>(pMsg));
      case 25:
        return ManageClientForceExitRequest();
      case 30:
        return CashDBInfoRecvResult(static_cast<int>(n), reinterpret_cast<const _cashdb_info_recv_result_acwr *>(pMsg));
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
        return WorldServiceInform(n, reinterpret_cast<const _world_service_inform_acwr *>(pMsg));
      case 3u:
        return WorldExitInform(n);
      case 5u:
        return WorldMsgInform(n, reinterpret_cast<const _world_msg_inform_acwr *>(pMsg));
      default:
        return type1 == 7 && ChatLockCommand(n, reinterpret_cast<const _chat_lock_command_acwr *>(pMsg));
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
      return BillingCloseRequest(n, reinterpret_cast<const _billing_close_request_bizo *>(pMsg));
    case 8u:
      return BillingRemaintimePersonal(n, reinterpret_cast<const _remaintime_personal_request_bizo *>(pMsg));
    case 9u:
      return BillingRemaintimePCBang(n, reinterpret_cast<const _remaintime_pcbang_request_bizo *>(pMsg));
    case 0xAu:
      return BillingChangeType(n, reinterpret_cast<const _change_type_request_bizo *>(pMsg));
    case 0xBu:
      return BillingExpirePersonal(n, reinterpret_cast<const _expire_personal_request_bizo *>(pMsg));
    case 0xCu:
      return BillingExpirePCBang(n, reinterpret_cast<const _expire_pcbang_request_bizo *>(pMsg));
    case 0xDu:
      return BillingExpireIPOverflow(n, reinterpret_cast<const _expire_ipoverflow_request_bizo *>(pMsg));
    case 0xEu:
      return BillingDestroyModule(n);
    case 0xFu:
      return TaiwanBillingUserCertify(n, pMsg);
    case 0x11u:
      return ChinaBillingChangePrimium(n, pMsg);
    default:
      break;
  }
  return pMsgHeader->m_byType[1] != 90
      || ZoneAliveCheckRequest(n, reinterpret_cast<const _zone_alive_check_request_bizo *>(pMsg));
}

bool CNetworkEX::WebAgentLineAnalysis(int n, _MSG_HEADER *pMsgHeader, char *pMsg)
{
  if (pMsgHeader->m_byType[0] == 51)
  {
    const unsigned __int8 type1 = pMsgHeader->m_byType[1];
    if (!type1)
    {
      return LogInWebAgentServer(static_cast<unsigned int>(n), reinterpret_cast<const _login_web_agent_server_request_wazo *>(pMsg));
    }
    if (type1 == 12)
    {
      return SendRaceBossMsgFromWebRequest(n, reinterpret_cast<const _send_race_boss_msg_from_web_request_wazo *>(pMsg));
    }
  }
  if (pMsgHeader->m_byType[0] != 54)
  {
    return true;
  }

  const unsigned __int8 type1 = pMsgHeader->m_byType[1];
  if (!type1)
  {
    return LogInControllServer(static_cast<unsigned int>(n), reinterpret_cast<const _login_controll_server_request_wazo *>(pMsg));
  }
  if (type1 == 3)
  {
    return CancelRaceBossSMSMsg(n, pMsg);
  }
  return type1 != 5 || ConnectionStatusRequest(n);
}

bool CNetworkEX::OpenWorldSuccessResult(unsigned int n, const _open_world_success_acwr *request)
{
  if (g_Main.m_byWorldType == request->byWorldType)
  {
    g_Main.pc_OpenWorldSuccessResult(
      request->byWorldCode,
      const_cast<char *>(request->szDBName),
      const_cast<char *>(request->szDBIP));
  }
  else
  {
    MyMessageBox(
      "OpenWorld(Account -> Zone)",
      "!!Server type is wrong!!(AC:%d)(ZO:%d)",
      request->byWorldType,
      g_Main.m_byWorldType);
    WriteServerStartHistory(
      "ServerType is Wrong ==> AccountServer(%d) != ZoneServer(%d)",
      request->byWorldType,
      g_Main.m_byWorldType);
  }
  return true;
}

bool CNetworkEX::OpenWorldFailureResult(unsigned int n, const _open_world_failure_acwr *request)
{
  g_Main.pc_OpenWorldFailureResult(const_cast<char *>(request->szErrCode));
  return true;
}

bool CNetworkEX::ForceCloseCommand(unsigned int n, const _force_close_command_acwr *request)
{
  if (request->idLocal.wIndex > 0x9E4u)
  {
    return false;
  }
  g_Main.pc_ForceCloseCommand(
    const_cast<_CLID *>(&request->idLocal),
    request->bDirectly,
    request->byKickType,
    request->dwPushIP);
  return true;
}

bool CNetworkEX::TransAccountInform(unsigned int n, const _trans_account_inform_acwr *request)
{
  g_Main.pc_TransIPKeyInform(
    request->dwAccountSerial,
    const_cast<char *>(request->szAccountID),
    request->byUserDgr,
    request->bySubDgr,
    const_cast<unsigned int *>(request->dwKey),
    const_cast<_GLBID *>(&request->gidGlobal),
    request->dwClientIP,
    request->bChatLock,
    request->iType,
    const_cast<char *>(request->szCMS),
    const_cast<_SYSTEMTIME *>(&request->stEndDate),
    request->lRemainTime,
    request->byUILock,
    const_cast<char *>(request->szUILock_pw),
    request->byUILock_failcnt,
    const_cast<char *>(request->szAccount_pw),
    request->byUILock_HintIndex,
    const_cast<char *>(request->uszUILock_HintAnswer),
    request->byUILockFindPassFailCount,
    request->bIsPcBang,
    request->nTrans,
    request->bAgeLimit,
    const_cast<unsigned int *>(request->dwRequestMoveCharacterSerialList),
    const_cast<unsigned int *>(request->dwTournamentCharacterSerialList));
  return true;
}

bool CNetworkEX::EnterWorldResult(unsigned int n, const _enter_world_result_acwr *request)
{
  if (request->idLocal.wIndex > 0x9E4u)
  {
    return false;
  }
  g_Main.pc_EnterWorldResult(request->byRetCode, const_cast<_CLID *>(&request->idLocal));
  return true;
}

bool CNetworkEX::UILockInitResult(unsigned int n, const _uilock_init_result_acwr *request)
{
if (request->wUserIndex >= 0x9E4u)
  {
    return false;
  }
  g_Main.pc_UILockInitResult(request);
  return true;
}

bool CNetworkEX::UILockUpdateResult(unsigned int n, const _uilock_update_result_acwr *request)
{
if (request->wUserIndex >= 0x9E4u)
  {
    return false;
  }
  g_Main.pc_UILockUpdateResult(request);
  return true;
}

bool CNetworkEX::UILockRefreshResult(unsigned int n, const _uilock_user_refresh_info_result_acwr *request)
{
if (request->byResult)
  {
    g_Main.m_logSystemError.Write(
      "_uilock_update_result_acwr ret(%u) Account(%u) Fail!",
      request->byResult,
      request->dwAccountSerial);
  }
  return true;
}

bool CNetworkEX::CheckIsBlockIPResult(unsigned int n, const _check_is_block_ip_result_acwr *request)
{
  unsigned __int8 *ipBytes = reinterpret_cast<unsigned __int8 *>(const_cast<unsigned int *>(&request->ulIP));
  const unsigned __int16 index = request->idLocal.wIndex;
  if (index < MAX_PLAYER)
  {
    CUserDB *user = &g_UserDB[index];
    unsigned __int8 kickType = 11;
    if (user->m_idWorld.dwSerial == static_cast<unsigned int>(-1))
    {
      return true;
    }

    if (user->m_idWorld.dwSerial != request->idLocal.dwSerial)
    {
      g_Main.m_logSystemError.Write(
        "CNetworkEX::CheckIsBlockIPResult(...) : \r\n"
        "IP(%u.%u.%u.%u) pUser->m_idWorld.dwSerial(%u) != pRecv->idLocal.dwSerial(%u) Invalid!",
        ipBytes[0],
        ipBytes[1],
        ipBytes[2],
        ipBytes[3],
        user->m_idWorld.dwSerial,
        request->idLocal.dwSerial);
    }

    const unsigned __int8 retCode = request->byRet;
    if (retCode)
    {
      switch (retCode)
      {
        case 1:
          kickType = 10;
          break;
        case 2:
          g_Main.m_logSystemError.Write(
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
          g_Main.m_logSystemError.Write(
            "CNetworkEX::CheckIsBlockIPResult(...) : \r\nIP(%u.%u.%u.%u) Account : %u(%s) DB Error!",
            ipBytes[0],
            ipBytes[1],
            ipBytes[2],
            ipBytes[3],
            user->m_dwAccountSerial,
            user->m_szAccountID);
          break;
        default:
          g_Main.m_logSystemError.Write(
            "CNetworkEX::CheckIsBlockIPResult(...) : \r\nIP(%u.%u.%u.%u) Account : %u(%s) Invalid Return(%u) Value!",
            ipBytes[0],
            ipBytes[1],
            ipBytes[2],
            ipBytes[3],
            user->m_dwAccountSerial,
            user->m_szAccountID,
            retCode);
          break;
      }

      _server_notify_inform_zone notify{};
      memset_0(&notify.dwPushIP, 0, sizeof(notify.dwPushIP));
      notify.wMsgCode = 10;
      notify.dwPushIP = request->ulIP;
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
      g_Network.Close( 0, user->m_idWorld.wIndex, true, buffer);
      return true;
    }

    return true;
  }

  g_Main.m_logSystemError.Write(
    "CNetworkEX::CheckIsBlockIPResult(...) : \r\nIP(%u.%u.%u.%u) pRecv->idLocal.wIndex(%u) >= MAX_PLAYER(%u) Invalid!",
    ipBytes[0],
    ipBytes[1],
    ipBytes[2],
    ipBytes[3],
    index,
    MAX_PLAYER);
  return true;
}

bool CNetworkEX::ConEventTotalSalesCheck(int n)
{
g_Main._CheckTotalSales();
  return true;
}

bool CNetworkEX::DisconnectGuildWarCharacterRequest(int n, const _disconnect_guild_war_character_request_acwr *request)
{
_character_disconnect_result_wrac result{};
  result.wClientIndex = request->wClientIndex;
  CUserDB *user = SearchAvatorWithName(g_UserDB, MAX_PLAYER, const_cast<char *>(request->szCharacterName));
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

bool CNetworkEX::ManageClientLimitRunRequest(const _manage_client_limit_run_request_acwr *request)
{
  g_Main.ManageClientLimitRunRequest(request);
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

bool CNetworkEX::CashDBInfoRecvResult(int n, const _cashdb_info_recv_result_acwr *request)
{
  g_Main.pc_CashDBInfoRecvResult(
    const_cast<char *>(request->szIP),
    const_cast<char *>(request->szDBName),
    const_cast<char *>(request->szAccount),
    const_cast<char *>(request->szPassword),
    request->dwPort);
  return true;
}

bool CNetworkEX::WorldServiceInform(unsigned int n, const _world_service_inform_acwr *request)
{
g_Main.pc_AlterWorldService(request->bService);
  return true;
}

bool CNetworkEX::WorldExitInform(unsigned int n)
{
g_Main.pc_AllUserKickInform();
  return true;
}

bool CNetworkEX::WorldMsgInform(unsigned int n, const _world_msg_inform_acwr *request)
{
char msgBuf[1312]{};
  const unsigned __int16 msgLen = request->wMsgSize;
  if (msgLen < 0x500u)
  {
    memcpy_0(msgBuf, request->wszMsg, msgLen);
    msgBuf[msgLen] = 0;
    g_Main.pc_AllUserMsgInform( msgBuf);
  }
  return true;
}

bool CNetworkEX::ChatLockCommand(unsigned int n, const _chat_lock_command_acwr *request)
{
g_Main.pc_ChatLockCommand(const_cast<_CLID *>(&request->idLocal), request->wBlockTimeH);
  return true;
}

bool CNetworkEX::BillingCloseRequest(int n, const _billing_close_request_bizo *request)
{
CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->BillingClose(const_cast<char *>(request->szID));
  return true;
}

bool CNetworkEX::BillingRemaintimePersonal(int n, const _remaintime_personal_request_bizo *request)
{
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Remaintime_Personal(
    const_cast<char *>(request->szAccount),
    request->iType,
    request->lRemainTime,
    const_cast<_SYSTEMTIME *>(&request->stEndDate));
  return true;
}

bool CNetworkEX::BillingRemaintimePCBang(int n, const _remaintime_pcbang_request_bizo *request)
{
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Remaintime_PCBang(
    const_cast<char *>(request->szCMSCode),
    request->iType,
    request->lRemainTime,
    const_cast<_SYSTEMTIME *>(&request->stEndDate));
  return true;
}

bool CNetworkEX::BillingChangeType(int n, const _change_type_request_bizo *request)
{
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Change_BillingType(
    const_cast<char *>(request->szAccount),
    const_cast<char *>(request->szCMSCode),
    request->iType,
    request->lRemainTime,
    const_cast<_SYSTEMTIME *>(&request->stEndDate),
    static_cast<unsigned __int8>(request->byReason));
  return true;
}

bool CNetworkEX::BillingExpirePersonal(int n, const _expire_personal_request_bizo *request)
{
CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Expire_Personal(const_cast<char *>(request->szAccount));
  return true;
}

bool CNetworkEX::BillingExpirePCBang(int n, const _expire_pcbang_request_bizo *request)
{
CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Expire_PCBang(const_cast<char *>(request->szCMSCode));
  return true;
}

bool CNetworkEX::BillingExpireIPOverflow(int n, const _expire_ipoverflow_request_bizo *request)
{
CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Expire_IPOverflow(const_cast<char *>(request->szAccount));
  return true;
}

bool CNetworkEX::BillingDestroyModule(int n)
{
g_Main.EndServer();
  return true;
}

bool CNetworkEX::TaiwanBillingUserCertify(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_taiwan_billing_user_certify_request_bizo *>(pBuf);
  g_Main.pc_TaiwanBillingUserCertify(
    request->szAccount,
    static_cast<unsigned __int8>(request->byCertify));
  return true;
}

bool CNetworkEX::ChinaBillingChangePrimium(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_china_billing_change_primium_request_bizo *>(pBuf);
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->Change_Primium(
    request->szID,
    static_cast<unsigned __int8>(request->byChangeType));
  return true;
}

bool CNetworkEX::ZoneAliveCheckRequest(int n, const _zone_alive_check_request_bizo *request)
{
  CBillingManager *billing = CTSingleton<CBillingManager>::Instance();
  billing->SendMsg_ZoneAliveCheck(request->dwTick);
  return true;
}

bool CNetworkEX::LogInWebAgentServer(unsigned int n, const _login_web_agent_server_request_wazo *request)
{
  unsigned __int8 type[2]{51, 1};
  _login_zone_result_zoct result{};
  if (g_Main.m_bConnectedWebAgentServer)
  {
    result.byRet = 1;
    g_Network.m_pProcess[2]->LoadSendMsg(
      n,
      type,
      reinterpret_cast<char *>(&result),
      result.size());
    return true;
  }

  if (static_cast<unsigned __int8>(request->byLoginCode) == 237)
  {
    g_Main.m_bConnectedWebAgentServer = true;
    g_Main.m_byWebAgentServerNetInx = static_cast<unsigned __int8>(n);
  }
  else
  {
    result.byRet = 2;
  }
  g_Network.m_pProcess[2]->LoadSendMsg(
    n,
    type,
    reinterpret_cast<char *>(&result),
    result.size());
  return true;
}

bool CNetworkEX::SendRaceBossMsgFromWebRequest(int n, const _send_race_boss_msg_from_web_request_wazo *request)
{
  _qry_case_sendwebracebosssms qry{};
  qry.dwWebSendDBID = request->dwWebSendDBID;
  strcpy_0(qry.szCharacterName, request->szCharacterName);
  strcpy_0(qry.wszMsg, request->wszMsg);
  const unsigned __int16 size = qry.size();
  g_Main.PushDQSData( 0, nullptr, 0x19u, reinterpret_cast<char *>(&qry), size);
  return true;
}

bool CNetworkEX::LogInControllServer(unsigned int n, const _login_controll_server_request_wazo *request)
{
  unsigned __int8 type[2]{54, 1};
  _login_zone_result_zoct result{};
  if (g_Main.m_bConnectedControllServer || static_cast<unsigned __int8>(request->byLoginCode) != 239)
  {
    result.byRet = 1;
    g_Network.m_pProcess[2]->LoadSendMsg(
      n,
      type,
      reinterpret_cast<char *>(&result),
      result.size());
    return false;
  }

  g_Main.m_bConnectedControllServer = true;
  g_Main.m_byControllServerNetInx = static_cast<unsigned __int8>(n);
  g_Network.m_pProcess[2]->LoadSendMsg(
    n,
    type,
    reinterpret_cast<char *>(&result),
    result.size());
  return true;
}

char CNetworkEX::DarkHoleOpenRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_darkhole_open_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_DarkHoleOpenRequest(static_cast<unsigned __int16>(request->dwItemSerial));
  }
  return 1;
}

char CNetworkEX::DarkHoleEnterRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_darkhole_enter_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int16 holeIndex = request->wHoleIndex;
  const unsigned int holeSerial = request->dwHoleSerial;
  if (holeIndex < MAX_DARKHOLE)
  {
    player->pc_DarkHoleEnterRequest(holeIndex, holeSerial);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: DarkHoleEnterRequest()..  if(pRecv->wHoleIndex >= MAX_DARKHOLE)",
    charName);
  return 0;
}

char CNetworkEX::DarkHoleGiveupOutRequest(int n, char *pBuf)
{
// this is not a stub
  return 1;
}

char CNetworkEX::DarkHoleClearOutRequest(int n, char *pBuf)
{
// this is not a stub
  return 1;
}

char CNetworkEX::DarkHoleAnswerReenterRequest(int n, char *pBuf)
{
// this is not a stub
  return 1;
}

char CNetworkEX::BossSMSMsgRequest(int n, char *pBuf)
{
  CPlayer *sender = &g_Player[n];
  auto *request = reinterpret_cast<_boss_sms_msg_request_clzo *>(pBuf);
  if (!sender->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 msgSize = static_cast<unsigned __int8>(request->bySize);
  if (msgSize <= 0x30u)
  {
    if (IsSQLValidString(request->wszChatData))
    {
      char msg[72]{};
      memcpy_0(msg, request->wszChatData, msgSize);
      msg[msgSize] = 0;
      CRaceBossMsgController::Instance()->Send(sender, msg);
      return 1;
    }

    const char *invalidMsg = request->wszChatData;
    const char *charName = sender->m_Param.GetCharNameA();
    g_Main.m_logSystemError.Write(
      "CNetworkEX::BossSMSMsgRequest() : %u(%s) !::IsSQLValidString( pRecv->wszChatData(%s) ) Invalid!",
      sender->m_dwObjSerial,
      charName,
      invalidMsg);
    return 0;
  }

  const char *charName = sender->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: BossSMSMsgRequest()..  if(pRecv->bySize > max_boss_sms_msg_size)",
    charName);
  return 0;
}

char CNetworkEX::PostSendRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_post_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    const unsigned __int8 race = static_cast<unsigned __int8>(player->m_Param.GetRaceCode());
    CPostSystemManager::Instace()->PostSendRequest(
      player,
      request->wszRecvName,
      request->wszTitle,
      request->wszContent,
      &request->Item,
      request->dwGold,
      race);
  }
  return 1;
}

char CNetworkEX::PostListRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (player->m_bLoad)
  {
    player->pc_PostListRequest();
  }
  return 1;
}

char CNetworkEX::PostContentRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_post_index_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_PostContentRequest(request->dwPostIndex);
  }
  return 1;
}

char CNetworkEX::PostItemGoldRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_post_index_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_PostItemGoldRequest(request->dwPostIndex);
  }
  return 1;
}

char CNetworkEX::PostDeleteRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_post_index_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_PostDeleteRequest(request->dwPostIndex);
  }
  return 1;
}

char CNetworkEX::PostReturnConfirmRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_post_return_confirm_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_PostReturnConfirmRequest(request->dwPostSerial);
  }
  return 1;
}

bool CNetworkEX::CancelRaceBossSMSMsg(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_cancel_raceboss_msg_request_ctzo *>(pBuf);
  if (!g_Main.m_bConnectedControllServer)
  {
    return false;
  }

  unsigned __int8 type[2]{54, 4};
  _cancel_raceboss_msg_result_zoct result{};
  result.byRet = 1;
  result.byRaceCode = static_cast<unsigned __int8>(request->byRaceCode);
  result.nID = request->nID;

  CRaceBossMsgController *controller = CRaceBossMsgController::Instance();
  if (controller->Cancel(request->byRaceCode, request->nID))
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

char CNetworkEX::ForceInvenChangeRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_force_inven_change_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  if (request->Item.byStorageCode == 0 || request->Item.byStorageCode == 3)
  {
    player->pc_ForceInvenChange(&request->Item, request->wReplaceSerial);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ForceInvenChangeRequest() : pRecv->Item.byStorageCode(%d)",
    charName,
    request->Item.byStorageCode);
  return 0;
}

char CNetworkEX::AnimusInvenChangeRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_animus_inven_change_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  if (request->Item.byStorageCode == 0 || request->Item.byStorageCode == 4)
  {
    player->pc_AnimusInvenChange(&request->Item, request->wReplaceSerial);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: AnimusInvenChangeRequest() : pRecv->Item.byStorageCode(%d)",
    charName,
    request->Item.byStorageCode);
  return 0;
}

char CNetworkEX::ResSeparationRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_res_separation_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  const unsigned __int8 moveAmount = static_cast<unsigned __int8>(request->byMoveAmount);
  if (moveAmount < 0x63u)
  {
    player->pc_ResSeparation(request->wStartSerial, moveAmount);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ResSeparationRequest() : pRecv->byMoveAmount(%d)",
    charName,
    moveAmount);
  return 0;
}

char CNetworkEX::ResDivisionRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_res_division_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  const unsigned __int8 moveAmount = static_cast<unsigned __int8>(request->byMoveAmount);
  if (moveAmount < 0x63u)
  {
    player->pc_ResDivision(
      request->wStartSerial,
      request->wTarSerial,
      moveAmount);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ResDivisionRequest() : pRecv->byMoveAmount(%d)",
    charName,
    moveAmount);
  return 0;
}

char CNetworkEX::PotionSocketSeparationRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_potionsocket_separation_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  const unsigned __int8 moveAmount = static_cast<unsigned __int8>(request->byMoveAmount);
  if (moveAmount <= 0x63u)
  {
    player->pc_PotionSeparation(request->wStartSerial, moveAmount);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: PotionSocketSeparationRequest() : pRecv->byMoveAmount(%d)",
    charName,
    moveAmount);
  return 0;
}

char CNetworkEX::PotionSocketDivisionRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_potionsocket_division_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  const unsigned __int8 moveAmount = static_cast<unsigned __int8>(request->byMoveAmount);
  if (moveAmount <= 0x63u)
  {
    player->pc_PotionDivision(
      request->wStartSerial,
      request->wTarSerial,
      moveAmount);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: PotionSocketDivisionRequest() : pRecv->byMoveAmount(%d)",
    charName,
    moveAmount);
  return 0;
}

char CNetworkEX::AlterItemSlotRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_alter_item_slot_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 count = static_cast<unsigned __int8>(request->byNum);
  if (count == 0 || count > 0x64u)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: AlterItemSlotRequest() : pRecv->byNum(%d)",
      charName,
      count);
    return 0;
  }

  for (int index = 0; index < count; ++index)
  {
    const unsigned __int8 storageIndex = static_cast<unsigned __int8>(request->list[index].byStorageIndex);
    const unsigned __int8 clientSlotIndex = static_cast<unsigned __int8>(request->list[index].byClientSlotIndex);

    if (storageIndex)
    {
      if (storageIndex != 2)
      {
        const char *charName = player->m_Param.GetCharNameA();
        m_LogFile.Write(
          "odd.. %s: AlterItemSlotRequest() : pRecv->list[%d].byStorageIndex(%d)",
          charName,
          index,
          storageIndex);
        return 0;
      }

      if (clientSlotIndex >= 7u)
      {
        const char *charName = player->m_Param.GetCharNameA();
        m_LogFile.Write(
          "odd.. %s: AlterItemSlotRequest() : pRecv->list[%d].byStorageIndex(%d), pRecv->list[%d].byClientSlotIndex(%d)",
          charName,
          index,
          storageIndex,
          index,
          clientSlotIndex);
        return 0;
      }
    }
    else if (clientSlotIndex >= 0x64u)
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: AlterItemSlotRequest() : pRecv->list[%d].byStorageIndex(%d), pRecv->list[%d].byClientSlotIndex(%d)",
        charName,
        index,
        storageIndex,
        index,
        clientSlotIndex);
      return 0;
    }
  }

  player->pc_AlterItemSlotRequest(
    count,
    request->list);
  return 1;
}

char CNetworkEX::AlterLinkBoardSlotRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_alter_link_slot_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 count = static_cast<unsigned __int8>(request->byNum);
  if (count >= 0x32u)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: AlterLinkBoardSlotRequest() : pRecv->byNum(%d)",
      charName,
      count);
    return 0;
  }

  for (int index = 0; index < count; ++index)
  {
    const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->list[index].bySlotIndex);
    const unsigned __int8 linkCode = static_cast<unsigned __int8>(request->list[index].byLinkCode);

    if (slotIndex >= 0x32u)
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: AlterLinkBoardSlotRequest() : pRecv->list[%d].bySlotIndex(%d)",
        charName,
        index,
        slotIndex);
      return 0;
    }

    if (linkCode != 0xFF && linkCode >= 7u)
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: AlterLinkBoardSlotRequest() : pRecv->list[%d].byLinkCode(%d)",
        charName,
        index,
        linkCode);
      return 0;
    }
  }

  player->pc_AlterLinkBoardSlotRequest(
    count,
    request->list,
    static_cast<unsigned __int8>(request->byLinkLock));
  return 1;
}

char CNetworkEX::PvpRankListRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_pvp_rank_list_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 byRace = static_cast<unsigned __int8>(request->byRace);
  const unsigned __int8 byPage = static_cast<unsigned __int8>(request->byPage);
  if (byRace < 3u)
  {
    if (byPage < 0xAu)
    {
      CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
      ranking->PvpRankListRequest(
        player->m_ObjID.m_wIndex,
        byRace,
        static_cast<unsigned __int8>(request->byType),
        byPage);
      return 1;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: PvpRankListRequest() : pRecv->byPage(%d)",
      charName,
      byPage);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: PvpRankListRequest() : pRecv->byRace(%d)",
    charName,
    byRace);
  return 0;
}

char CNetworkEX::ModeChangeRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_mode_change_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 modeCode = static_cast<unsigned __int8>(request->byModeCode);
  const unsigned __int8 standCode = static_cast<unsigned __int8>(request->byStandCode);

  if (modeCode == 0 || modeCode == 1)
  {
    if (standCode == 0 || standCode == 1)
    {
      player->pc_ChangeModeType(modeCode, standCode);
      return 1;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ModeChangeRequest() : pRecv->byStandCode(%d)",
      charName,
      standCode);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ModeChangeRequest() : pRecv->byModeCode(%d)",
    charName,
    modeCode);
  return 0;
}

char CNetworkEX::GustureRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_gesture_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 gestureType = static_cast<unsigned __int8>(request->byGestureType);
  if (gestureType < 0x64u)
  {
    player->pc_GestureRequest(gestureType);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: GustureRequest() : pRecv->byGestureType(%d)",
    charName,
    gestureType);
  return 0;
}

char CNetworkEX::AlterWindowInfoRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_alter_window_info_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    player->pc_AlterWindowInfoRequest(
      request->dwSkill,
      request->dwForce,
      request->dwCharacter,
      request->dwAnimus,
      request->dwInven,
      request->dwInvenBag);
  }
  return 1;
}

char CNetworkEX::SetTargetObjectRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_set_target_object_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    CGameObject *target = g_Main.GetObjectA(
      static_cast<unsigned __int8>(request->byObjKind),
      static_cast<unsigned __int8>(request->byID),
      request->wIndex);
    player->pc_SetTargetObjectRequest(target, request->dwObjSerial, 0);
  }
  return 1;
}

char CNetworkEX::ReleaseTargetObjectRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_ReleaseTargetObjectRequest();
  }
  return 1;
}

char CNetworkEX::PartyReqBlockReport(int n, const _party_req_block_report_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_PartyReqBlock(request->bBlock);
  }
  return 1;
}

char CNetworkEX::WhisperBlockReport(int n, const _whisper_block_report_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_WhisperBlock(request->bBlock);
  }
  return 1;
}

char CNetworkEX::TradeBlockReport(int n, const _trade_block_report_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_TradeBlock(request->bBlock);
  }
  return 1;
}

char CNetworkEX::GuildBattleBlockReport(int n, const _guild_battle_block_report_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_GuildBattleBlock(request->bBlock);
  }
  return 1;
}

char CNetworkEX::PlayerMacroUpdate(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_MacroUpdate(pBuf);
  }
  return 1;
}

char CNetworkEX::TotalGuildRankRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_total_guild_rank_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    CTotalGuildRankManager *manager = CTotalGuildRankManager::Instance();
    manager->Send(
      request->dwVer,
      static_cast<unsigned __int8>(request->byRace),
      player);
  }
  return 1;
}

char CNetworkEX::WeeklyGuildRankRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_weekly_guild_rank_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    CWeeklyGuildRankManager *manager = CWeeklyGuildRankManager::Instance();
    manager->Send(
      request->dwVer,
      static_cast<unsigned __int8>(request->byRace),
      player);
  }
  return 1;
}

char CNetworkEX::SetRaceBossCryMsgRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_set_race_boss_cry_msg_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->bySlot);
  if (slotIndex >= 0xAu)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: SetRaceBossCryMsgRequest()..  if( pRecv->bySlot >= max_cry_msg || pRecv->bySlot < 0)",
      charName);
    return 0;
  }

  char message[65];
  memset(message, 0, sizeof(message));
  strncpy_s(message, request->wszMessage, 0x40uLL);
  player->pc_SetRaceBossCryMsg(slotIndex, message);
  return 1;
}

char CNetworkEX::NotifyRaceBossCryMsg(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  player->pc_NotifyRaceBossCryMsg();
  return 1;
}

char CNetworkEX::BillingInfoRequest(int n, char *pBuf)
{
CUserDB *user = &g_UserDB[n];
  if (user)
  {
    user->SendMsg_BillingInfo();
  }

  return 1;
}

char CNetworkEX::ReleaseSiegeModeRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_ReleaseSiegeModeRequest();
  return 1;
}

char CNetworkEX::TransformSiegeModeRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_transform_siege_mode_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_TransformSiegeModeRequest(request->wItemSerial);
  return 1;
}

char CNetworkEX::TransShipRenewTicketRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_transship_renew_ticket_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_TransShipRenewTicketRequest(request->wTicketSerial);
  }

  return 1;
}

char CNetworkEX::TrunkEstRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_trunk_est_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  char password[128]{};
  memcpy_0(password, request->wszPassword, 0xCu);
  password[12] = 0;

  const unsigned __int8 hintIndex = static_cast<unsigned __int8>(request->byHintIndex);

  char hintAnswer[128]{};
  memcpy_0(hintAnswer, request->wszHintAnswer, 0x10u);
  hintAnswer[16] = 0;

  player->pc_TrunkEstRequest(password, hintIndex, hintAnswer);
  return 1;
}

char CNetworkEX::TrunkDownloadRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_trunk_download_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  char password[128]{};
  memcpy_0(password, request->wszPassword, 0xCu);
  password[12] = 0;

  player->pc_TrunkDownloadRequest(password);
  return 1;
}

char CNetworkEX::TrunkChangePasswdRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_trunk_change_passwd_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  char prevPassword[128]{};
  memcpy_0(prevPassword, request->wszPrevPassword, 0xCu);
  prevPassword[12] = 0;

  char changedPassword[128]{};
  memcpy_0(changedPassword, request->wszChangedPassword, 0xCu);
  changedPassword[12] = 0;

  const unsigned __int8 hintIndex = static_cast<unsigned __int8>(request->byHintIndex);

  char hintAnswer[128]{};
  memcpy_0(hintAnswer, request->wszHintAnswer, 0x10u);
  hintAnswer[16] = 0;

  player->pc_TrunkChangePasswdRequest(prevPassword, changedPassword, hintIndex, hintAnswer);
  return 1;
}

char CNetworkEX::TrunkExtendRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_TrunkExtendRequest();
  return 1;
}

char CNetworkEX::TrunkAlterItemSlotRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_trunk_alter_item_slot_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 clientSlotIndex = static_cast<unsigned __int8>(request->byClientSlotIndex);
  const unsigned __int8 storageIndex = static_cast<unsigned __int8>(request->byStorageIndex);
  if (storageIndex == 5)
  {
    if (clientSlotIndex >= 0x64u)
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: TrunkAlterItemSlotRequest() : if(pRecv->byClientSlotIndex >= trunk_storage_num)",
        charName);
      return 0;
    }
  }
  else if (storageIndex == 7)
  {
    if (clientSlotIndex >= 0x28u)
    {
      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: TrunkAlterItemSlotRequest() : if(pRecv->byClientSlotIndex >= extend_trunk_storage_num)",
        charName);
      return 0;
    }
  }
  else if (storageIndex >= 8u)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: TrunkAlterItemSlotRequest() : pRecv->byStorageIndex Is Invalid!",
      charName);
    return 0;
  }

  player->pc_TrunkAlterItemSlotRequest(
    request->dwItemSerial,
    clientSlotIndex,
    storageIndex);
  return 1;
}

char CNetworkEX::TrunkResDivisionRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_trunk_res_division_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  const unsigned __int16 moveAmount = request->wMoveAmount;
  if (moveAmount >= 0x63u)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: TrunkResDivisionRequest() : pRecv->wMoveAmount(%d)",
      charName,
      moveAmount);
    return 0;
  }

  const unsigned __int8 storageIndex = static_cast<unsigned __int8>(request->byStorageIndex);
  if (storageIndex >= 8u)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: TrunkResDivisionRequest() : pRecv->byStorageIndex Invalid!",
      charName);
    return 0;
  }

  player->pc_TrunkResDivision(
    request->wStartSerial,
    request->wTarSerial,
    moveAmount,
    storageIndex);
  return 1;
}

char CNetworkEX::TrunkPotionDivisionRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_trunk_potionsocket_division_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  const unsigned __int16 moveAmount = request->wMoveAmount;
  if (moveAmount >= 0x63u)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: TrunkPotionDivisionRequest() : pRecv->byMoveAmount(%d)",
      charName,
      moveAmount);
    return 0;
  }

  const unsigned __int8 storageIndex = static_cast<unsigned __int8>(request->byStorageIndex);
  if (storageIndex >= 8u)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: TrunkPotionDivisionRequest() : pRecv->byStorageIndex Invalid!",
      charName);
    return 0;
  }

  player->pc_TrunkPotionDivision(
    request->wStartSerial,
    request->wTarSerial,
    moveAmount,
    storageIndex);
  return 1;
}

char CNetworkEX::TrunkIoMoveRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_trunk_io_move_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 startStorage = static_cast<unsigned __int8>(request->byStartStorageIndex);
  const unsigned __int8 targetStorage = static_cast<unsigned __int8>(request->byTarStorageIndex);
  const char *charName = player->m_Param.GetCharNameA();

  if (targetStorage != 5 && startStorage != 5 && targetStorage != 7 && startStorage != 7)
  {
    m_LogFile.Write(
      "odd.. %s: TrunkIoMoveRequest() : if(pRecv->byTarStorageIndex != _STORAGE_POS::TRUNK && pRecv->byStartStorageIndex != _STORAGE_POS::TRUNK)",
      charName);
    return 0;
  }

  if ((targetStorage == 5 && startStorage == 5) || (targetStorage == 7 && startStorage == 7))
  {
    m_LogFile.Write(
      "odd.. %s: TrunkIoMoveRequest() : if(pRecv->byTarStorageIndex == _STORAGE_POS::TRUNK && pRecv->byStartStorageIndex == _STORAGE_POS::TRUNK)",
      charName);
    return 0;
  }

  if ((targetStorage == 5 && startStorage != 0 && startStorage != 1 && startStorage != 7 && startStorage != 2)
      || (targetStorage == 7 && startStorage != 0 && startStorage != 1 && startStorage != 5 && startStorage != 2))
  {
    m_LogFile.Write(
      "odd.. %s: TrunkIoMoveRequest() : if(pRecv->byStartStorageIndex != _STORAGE_POS::INVEN && pRecv->byStartStorageIndex != _STORAGE_POS::EQUIP",
      charName);
    return 0;
  }

  if ((startStorage == 5 && targetStorage != 0 && targetStorage != 1 && targetStorage != 7 && targetStorage != 2)
      || (startStorage == 7 && targetStorage != 0 && targetStorage != 1 && targetStorage != 5 && targetStorage != 2))
  {
    m_LogFile.Write(
      "odd.. %s: TrunkIoMoveRequest() : if(pRecv->byTarStorageIndex != _STORAGE_POS::INVEN && pRecv->byTarStorageIndex != _STORAGE_POS::EQUIP)",
      charName);
    return 0;
  }

  player->pc_TrunkIoMoveRequest(
    startStorage,
    targetStorage,
    request->wItemSerial,
    static_cast<unsigned __int8>(request->byClientSlotIndex));
  return 1;
}

char CNetworkEX::TrunkIoSwapRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_trunk_io_swap_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 startStorage = static_cast<unsigned __int8>(request->byStartStorageIndex);
  const unsigned __int8 targetStorage = static_cast<unsigned __int8>(request->byTarStorageIndex);
  const char *charName = player->m_Param.GetCharNameA();

  if (targetStorage != 5 && startStorage != 5 && targetStorage != 7 && startStorage != 7)
  {
    m_LogFile.Write(
      "odd.. %s: TrunkIoSwapRequest() : if(pRecv->byTarStorageIndex != _STORAGE_POS::TRUNK && pRecv->byStartStorageIndex != _STORAGE_POS::TRUNK)",
      charName);
    return 0;
  }

  if ((targetStorage == 5 && startStorage == 5) || (targetStorage == 7 && startStorage == 7))
  {
    m_LogFile.Write(
      "odd.. %s: TrunkIoSwapRequest() : if(pRecv->byTarStorageIndex == _STORAGE_POS::TRUNK && pRecv->byStartStorageIndex == _STORAGE_POS::TRUNK)",
      charName);
    return 0;
  }

  if ((targetStorage == 5 && startStorage != 0 && startStorage != 1 && startStorage != 7 && startStorage != 2)
      || (targetStorage == 7 && startStorage != 0 && startStorage != 1 && startStorage != 5 && startStorage != 2))
  {
    m_LogFile.Write(
      "odd.. %s: TrunkIoSwapRequest() : if(pRecv->byStartStorageIndex != _STORAGE_POS::INVEN && pRecv->byStartStorageIndex != _STORAGE_POS::EQUIP",
      charName);
    return 0;
  }

  if ((startStorage == 5 && targetStorage != 0 && targetStorage != 1 && targetStorage != 7 && targetStorage != 2)
      || (startStorage == 7 && targetStorage != 0 && targetStorage != 1 && targetStorage != 5 && targetStorage != 2))
  {
    m_LogFile.Write(
      "odd.. %s: TrunkIoSwapRequest() : if(pRecv->byTarStorageIndex != _STORAGE_POS::INVEN && pRecv->byTarStorageIndex != _STORAGE_POS::EQUIP)",
      charName);
    return 0;
  }

  player->pc_TrunkIoSwapRequest(
    startStorage,
    targetStorage,
    request->wStartSerial,
    request->wTarSerial);
  return 1;
}

char CNetworkEX::TrunkIoMergeRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_trunk_io_merge_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 startStorage = static_cast<unsigned __int8>(request->byStartStorageIndex);
  const unsigned __int8 targetStorage = static_cast<unsigned __int8>(request->byTarStorageIndex);
  const char *charName = player->m_Param.GetCharNameA();

  if (targetStorage != 5 && startStorage != 5 && targetStorage != 7 && startStorage != 7)
  {
    m_LogFile.Write(
      "odd.. %s: TrunkIoMergeRequest() : if(pRecv->byTarStorageIndex != _STORAGE_POS::TRUNK && pRecv->byStartStorageIndex != _STORAGE_POS::TRUNK)",
      charName);
    return 0;
  }

  if ((targetStorage == 5 && startStorage == 5) || (targetStorage == 7 && startStorage == 7))
  {
    m_LogFile.Write(
      "odd.. %s: TrunkIoMergeRequest() : if(pRecv->byTarStorageIndex == _STORAGE_POS::TRUNK && pRecv->byStartStorageIndex == _STORAGE_POS::TRUNK)",
      charName);
    return 0;
  }

  if ((targetStorage == 5 && startStorage != 0 && startStorage != 7)
      || (targetStorage == 7 && startStorage != 0 && startStorage != 5))
  {
    m_LogFile.Write(
      "odd.. %s: TrunkIoMergeRequest() : if(pRecv->byStartStorageIndex != _STORAGE_POS::INVEN)",
      charName);
    return 0;
  }

  if ((startStorage == 5 && targetStorage != 0 && targetStorage != 7)
      || (startStorage == 7 && targetStorage != 0 && targetStorage != 5))
  {
    m_LogFile.Write(
      "odd.. %s: TrunkIoMergeRequest() : if(pRecv->byTarStorageIndex != _STORAGE_POS::INVEN)",
      charName);
    return 0;
  }

  const unsigned __int16 moveAmount = request->wMoveAmount;
  if (moveAmount >= 0x63u || moveAmount == 0)
  {
    m_LogFile.Write(
      "odd.. %s: TrunkIoMergeRequest() : if(pRecv->wMoveAmount >= max_overlap_num || pRecv->wMoveAmount == 0)",
      charName);
    return 0;
  }

  player->pc_TrunkIoMergeRequest(
    startStorage,
    targetStorage,
    request->wStartSerial,
    request->wTarSerial,
    moveAmount);
  return 1;
}

char CNetworkEX::TrunkIoMoneyRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_trunk_io_money_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  const int dalant = static_cast<int>(request->dwDalant);
  const int gold = static_cast<int>(request->dwGold);
  if (dalant == 0 && gold == 0)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: TrunkIoMoneyRequest() : if(pRecv->dwDalant == 0 && pRecv->dwGold == 0)",
      charName);
    return 0;
  }

  player->pc_TrunkIoMoneyRequest(static_cast<unsigned __int8>(request->byCase), dalant, gold);
  return 1;
}

char CNetworkEX::TrunkPwHintIndexRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  player->pc_TrunkPwHintIndexRequest();
  return 1;
}

char CNetworkEX::TrunkHintAnswerRequest(int n, const _trunk_hint_answer_request_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  char answer[128]{};
  memcpy_0(answer, request->wszAnswer, sizeof(request->wszAnswer));
  answer[16] = 0;

  player->pc_TrunkHintAnswerRequest(answer);
  return 1;
}

char CNetworkEX::TrunkCreateCostIsFreeRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  _trunk_create_cost_is_free_result_zocl msg{};
  msg.byRet = static_cast<char>(player->pc_TrunkCreateCostIsFreeRequest());
  unsigned __int8 type[2] = {34, 25};
  g_Network.m_pProcess[0]->LoadSendMsg(
    player->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
  return 1;
}

char CNetworkEX::SetGroupTargetObjectRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_set_group_target_object_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 groupType = static_cast<unsigned __int8>(request->byGroupType);
  if (groupType < 3u)
  {
    CGameObject *target = g_Main.GetObjectA(
      0,
      static_cast<unsigned __int8>(request->byID),
      request->wIndex);
    player->pc_SetGroupTargetObjectRequest(target, request->dwObjSerial, groupType);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: SetGroupTargetObjectRequest()..  if(pRecv->byGroupType < 0 || pRecv->byGroupType >= GROUP_TYPE_NUM)",
    charName);
  return 0;
}

char CNetworkEX::ReleaseGroupTargetObjectRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_release_group_target_object_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 groupType = static_cast<unsigned __int8>(request->byGroupType);
  if (groupType < 3u)
  {
    player->pc_ReleaseGroupTargetObjectRequest(groupType);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ReleaseGroupTargetObjectRequest()..  if(pRecv->byGroupType < 0 || pRecv->byGroupType >= GROUP_TYPE_NUM)",
    charName);
  return 0;
}

char CNetworkEX::SetGroupMapPointRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_set_group_map_point_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 groupType = static_cast<unsigned __int8>(request->byGroupType);
  if (groupType < 3u)
  {
    player->pc_SetGroupMapPointRequest(groupType, request->zPos);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: SetGroupMapPointRequest()..  if(pRecv->byGroupType < 0 || pRecv->byGroupType >= GROUP_TYPE_NUM)",
    charName);
  return 0;
}

char CNetworkEX::RequestPatriarchPunishment(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_pt_request_punishment_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 punishmentType = static_cast<unsigned __int8>(request->byType);
  if (punishmentType < 3u)
  {
    const unsigned __int16 contentSize = request->wContentSize;
    if (contentSize <= 0x4FFu)
    {
      if (contentSize != 0)
      {
        char content[1296];
        memset(content, 0, 1280);
        memcpy_0(content, request->wszContent, contentSize);
        content[contentSize] = 0;
        player->pc_RequestPatriarchPunishment(punishmentType, request->wszCharName, content);
        return 1;
      }

      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: RequestPatriarchPunishment()..  if (pRecv->wContentSize == 0)",
        charName);
      return 0;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: RequestPatriarchPunishment()..  if (pRecv->wContentSize > _pt_request_punishment_clzo::content_size-1)",
      charName);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: RequestPatriarchPunishment()..  if (pRecv->byType < 0 || pRecv->byType >= PUNISHMENT_TYPE_NUM)",
    charName);
  return 0;
}

char CNetworkEX::RequestTaxRate(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_RequestTaxRate();
  }
  return 1;
}

char CNetworkEX::RequestChangeTaxRate(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_request_change_tax_rate_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    player->pc_RequestChangeTaxRate(static_cast<unsigned __int8>(request->byTaxRate));
  }
  return 1;
}

char CNetworkEX::RequestUILockInit(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  CUserDB *userDb = &g_UserDB[n];
  auto *request = reinterpret_cast<_uilock_init_request_clzo *>(pBuf);

  char uiLockPw[48];
  char uiLockPwConfirm[48];
  char uiLockHintAnswer[40];
  memset(uiLockPw, 0, 13);
  memset(uiLockPwConfirm, 0, 13);
  memset(uiLockHintAnswer, 0, 17);

  memcpy_0(uiLockPw, request->uszUILockPW, 0xCuLL);
  memcpy_0(uiLockPwConfirm, request->uszUILockPWConfirm, 0xCuLL);
  memcpy_0(uiLockHintAnswer, request->uszHintAnswer, 0x10uLL);

  player->pc_RequestUILockInit(
    userDb,
    uiLockPw,
    uiLockPwConfirm,
    static_cast<unsigned __int8>(request->byHintIndex),
    uiLockHintAnswer);
  return 1;
}

char CNetworkEX::RequestUILockUserCertify(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  CUserDB *userDb = &g_UserDB[n];
  player->pc_RequestUILockCertify(userDb, pBuf);
  return 1;
}

char CNetworkEX::RequestUILockUpdateInfo(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_uilock_update_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    player->pc_RequestUILockUpdate(
      request->uszUILockPWOld,
      request->uszUILockPW,
      request->uszUILockPWConfirm,
      static_cast<unsigned __int8>(request->byHintIndex),
      request->uszHintAnswer);
  }
  return 1;
}

char CNetworkEX::RequestUILockFindPW(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  CUserDB *userDb = &g_UserDB[n];
  player->pc_RequestUILockFindPW(userDb, pBuf);
  return 1;
}

char CNetworkEX::RequestTLLogoutTime(int n, char *pBuf)
{
CUserDB *userDb = &g_UserDB[n];
  if (!userDb)
  {
    return 0;
  }

  _qry_case_select_timelimit_info query{};
  query.dwAccountSerial = userDb->m_dwAccountSerial;
  query.wIndex = n;
  const int size = query.size();
  g_Main.PushDQSData( 0xFFFFFFFF, 0LL, 0x99u, reinterpret_cast<char *>(&query), size);
  return 1;
}

char CNetworkEX::MineStartRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_mine_start_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 mineIndex = static_cast<unsigned __int8>(request->byMineIndex);
  if (mineIndex < player->m_pCurMap->m_nResDumNum)
  {
    const unsigned __int8 oreIndex = static_cast<unsigned __int8>(request->byOreIndex);
    const int recordNum = g_Main.m_tblItemData[17].GetRecordNum();
    if (oreIndex < recordNum)
    {
      if (player->m_pCurMap)
      {
        player->pc_MineStart(mineIndex, oreIndex, request->wOreSerial);
        return 1;
      }

      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write("odd.. %s: MineStartRequest() .. if(!pOne->m_pCurMap)", charName);
      return 0;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: if(pRecv->byOreIndex >= g_Main.m_tblItemData[tbl_code_ore].GetRecordNum())",
      charName);
    return 0;
  }

  if (player->m_pCurMap->m_nResDumNum > 0)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: MineStartRequest() : pRecv->byMineIndex(%d) >= pOne->m_pCurMap->m_nResDumNum(%d)",
      charName,
      mineIndex,
      player->m_pCurMap->m_nResDumNum);
  }
  return 1;
}

char CNetworkEX::MineCancleRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_MineCancle();
  return 1;
}

char CNetworkEX::ResCuttingRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_ore_cutting_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 processNum = static_cast<unsigned __int8>(request->byProcessNum);
  if (processNum <= 0x63u)
  {
    player->pc_OreCutting(request->wOreSerial, processNum);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ResCuttingRequest()..  if(pRecv->byProcessNum > max_overlap_num)",
    charName);
  return 0;
}

char CNetworkEX::OreIntoBagRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_ore_into_bag_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 addAmount = static_cast<unsigned __int8>(request->byAddAmount);
  if (addAmount <= 0x63u)
  {
    const int resIndex = request->wResIndex;
    const int recordNum = g_Main.m_tblItemData[18].GetRecordNum();
    if (resIndex < recordNum)
    {
      player->pc_OreIntoBag(
        request->wResIndex,
        request->wSerial,
        addAmount);
      return 1;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: OreIntoBagRequest()..  if(pRecv->wResIndex >= g_Main.m_tblItemData[tbl_code_res].GetRecordNum())",
      charName);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: OreIntoBagRequest()..  if(pRecv->byAddAmount > max_overlap_num)",
    charName);
  return 0;
}

char CNetworkEX::CuttingCompleteRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_CuttingComplete(0);
  return 1;
}

char CNetworkEX::PartyJoinInvitation(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_party_join_invitation_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  if (player->m_EP.GetEff_Have(50) > 0.0)
  {
    player->SendMsg_JadeEffectErr(1u);
    return 1;
  }

  const unsigned __int16 targetIndex = request->wDstIndex;
  if (targetIndex < MAX_PLAYER)
  {
    player->pc_PartyJoinInvitation(targetIndex);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: PartyJoinInvitation()..  if(pRecv->wDstIndex >= MAX_PLAYER)",
    charName);
  return 0;
}

char CNetworkEX::PartyJoinInvitationAnswer(int n, _party_join_invitation_answer_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  if (request->idBoss.wIndex < MAX_PLAYER)
  {
    player->pc_PartyJoinInvitationAnswer(&request->idBoss);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: PartyJoinInvitationAnswer()..  if(pRecv->idBoss.wIndex >= MAX_PLAYER)",
    charName);
  return 0;
}

char CNetworkEX::PartyJoinApplication(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_party_join_application_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int16 bossIndex = request->wBossIndex;
  if (bossIndex < MAX_PLAYER)
  {
    player->pc_PartyJoinApplication(bossIndex);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: PartyJoinApplication()..  if(pRecv->wBossIndex >= MAX_PLAYER)",
    charName);
  return 0;
}

char CNetworkEX::PartyJoinApplicatiohAnswer(int n, _party_join_application_answer_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  if (request->idApplicant.wIndex < MAX_PLAYER)
  {
    player->pc_PartyJoinApplicationAnswer(&request->idApplicant);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: PartyJoinApplication()..  if(pRecv->idApplicant.wIndex >= MAX_PLAYER)",
    charName);
  return 0;
}

char CNetworkEX::PartyLeaveSelfRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_PartyLeaveSelfReqeuest();
  return 1;
}

char CNetworkEX::PartyLeaveCompulsionRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_party_leave_compulsion_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_PartyLeaveCompulsionReqeuest(request->dwExiterSerial);
  return 1;
}

char CNetworkEX::PartyDisjointRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_PartyDisJointReqeuest();
  return 1;
}

char CNetworkEX::PartySuccessionRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_party_succession_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_PartySuccessionReqeuest(request->dwSuccessorSerial);
  return 1;
}

char CNetworkEX::PartyLockRequest(int n, const _party_lock_request_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_PartyLockReqeuest(request->bLock);
  return 1;
}

char CNetworkEX::AlterPartyLootShareRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_alter_party_loot_share_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 lootShareMode = static_cast<unsigned __int8>(request->byLootShareMode);
  if (lootShareMode < 3u)
  {
    player->pc_PartyAlterLootShareReqeuest(lootShareMode);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: PartyJoinApplication()..  if(pRecv->byLootShareMode >= PARTY_ITEM_AUTHOR_TYPE_NUM)",
    charName);
  return 0;
}

char CNetworkEX::AwayPartyInvitation(int n, const _away_party_invitation_request_clzo *request)
{
  CPlayer *player = &g_Player[n];
  char inviteeName[17]{};
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  memcpy_0(inviteeName, request->wszCharName, sizeof(request->wszCharName));
  inviteeName[16] = 0;

  if (!strlen_0(inviteeName))
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: AwayPartyInvitation()..  if (__strlen(pRecv->wszCharName) == 0)",
      charName);
    return 0;
  }

  if (strlen_0(inviteeName) <= 0x10)
  {
    player->pc_AwaypartyInvitationRequest(inviteeName);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: AwayPartyInvitation()..  if (__strlen(wszCharName) > max_name_len)",
    charName);
  return 0;
}

char CNetworkEX::AwayPartyInvitationAnswer(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_away_party_invitation_answer_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int16 bossIndex = request->wBossIndex;
  if (bossIndex < MAX_PLAYER)
  {
    _CLID bossId{};
    bossId.wIndex = request->wBossIndex;
    bossId.dwSerial = request->dwBossSerial;
    player->pc_AwayPartyJoinInvitationAnswer(
      &bossId,
      static_cast<unsigned __int8>(request->byRetCode));
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: AwayPartyInvitationAnswer()..  if(pRecv->idBoss.wIndex >= MAX_PLAYER)",
    charName);
  return 0;
}

char CNetworkEX::ForceRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_force_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  if (g_Main.GetObjectA(0, static_cast<unsigned __int8>(request->byID), request->wIndex))
  {
    _CHRID targetId{};
    targetId.byID = request->byID;
    targetId.wIndex = request->wIndex;
    targetId.dwSerial = request->dwSerial;
    player->pc_ForceRequest(
      request->wForceSerial,
      &targetId,
      request->wConsumeItemSerial);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ForceRequest()..  if(!g_Main.GetObject(obj_kind_char, pRecv->idDst.byID, pRecv->idDst.wIndex))",
    charName);
  return 0;
}

char CNetworkEX::SkillRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_skill_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  if (g_Main.GetObjectA(
        0,
        static_cast<unsigned __int8>(request->idDst.byID),
        request->idDst.wIndex))
  {
    const int skillIndex = static_cast<unsigned __int8>(request->bySkillIndex);
    const int recordNum = g_Main.m_tblEffectData[0].GetRecordNum();
    if (skillIndex < recordNum)
    {
      player->pc_SkillRequest(
        static_cast<unsigned __int8>(request->bySkillIndex),
        &request->idDst,
        request->wConsumeItemSerial);
      return 1;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ForceRequest()..  if(pRecv->bySkillIndex >= g_Main.m_tblEffectData[effect_code_skill].GetRecordNum())",
      charName);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ForceRequest()..  if(!g_Main.GetObject(obj_kind_char, pRecv->idDst.byID, pRecv->idDst.wIndex))",
    charName);
  return 0;
}

char CNetworkEX::ClassSkillRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_class_skill_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  if (g_Main.GetObjectA(0, static_cast<unsigned __int8>(request->byID), request->wIndex))
  {
    const int skillIndex = request->wSkillIndex;
    const int recordNum = g_Main.m_tblEffectData[2].GetRecordNum();
    if (skillIndex < recordNum)
    {
      _CHRID targetId{};
      targetId.byID = request->byID;
      targetId.wIndex = request->wIndex;
      targetId.dwSerial = request->dwSerial;
      player->pc_ClassSkillRequest(
        request->wSkillIndex,
        &targetId,
        request->wConsumeItemSerial);
      return 1;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ClassSkillRequest()..  if(pRecv->wSkillIndex >= g_Main.m_tblEffectData[effect_code_class].GetRecordNum())",
      charName);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ClassSkillRequest()..  if(!g_Main.GetObject(obj_kind_char, pRecv->idDst.byID, pRecv->idDst.wIndex))",
    charName);
  return 0;
}

char CNetworkEX::SkillRecallTeleportRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_skill_recall_teleport_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const int skillIndex = request->wSkillIndex;
  const int recordNum = g_Main.m_tblEffectData[0].GetRecordNum();
  if (skillIndex < recordNum)
  {
    CPlayer *targetPlayer = GetPtrPlayerFromName(g_Player, MAX_PLAYER, request->wszRecallName);
    if (targetPlayer)
    {
      _CHRID targetId{};
      targetId.byID = targetPlayer->m_ObjID.m_byID;
      targetId.dwSerial = targetPlayer->m_ObjID.m_wIndex;
      targetId.wIndex = targetPlayer->m_dwObjSerial;
      player->pc_SkillRequest(
        static_cast<unsigned __int8>(request->wSkillIndex),
        &targetId,
        request->wConsumeItemSerial);
    }
    else
    {
      CRecallEffectController *controller = CRecallEffectController::Instance();
      controller->SendRecallReqeustResult(0x11u, player);
    }
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: SkillRecallTeleportRequest()..  if(pRecv->wSkillIndex >= g_Main.m_tblEffectData[effect_code_skill].GetRecordNum())",
    charName);
  return 0;
}

char CNetworkEX::ClassSkillRecallTeleportRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_class_skill_recall_teleport_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const int skillIndex = request->wSkillIndex;
  const int recordNum = g_Main.m_tblEffectData[0].GetRecordNum();
  if (skillIndex < recordNum)
  {
    CPlayer *targetPlayer = GetPtrPlayerFromName(g_Player, MAX_PLAYER, request->wszRecallName);
    if (targetPlayer)
    {
      _CHRID targetId{};
      targetId.byID = targetPlayer->m_ObjID.m_byID;
      targetId.dwSerial = targetPlayer->m_ObjID.m_wIndex;
      targetId.wIndex = targetPlayer->m_dwObjSerial;
      player->pc_ClassSkillRequest(request->wSkillIndex, &targetId, request->wConsumeItemSerial);
    }
    else
    {
      CRecallEffectController *controller = CRecallEffectController::Instance();
      controller->SendRecallReqeustResult(0x11u, player);
    }
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ClassSkillRecallTeleportRequest()..  if(pRecv->wSkillIndex >= g_Main.m_tblEffectData[effect_code_skill].GetRecordNum())",
    charName);
  return 0;
}

char CNetworkEX::ThrowSkillRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_throw_skill_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  if (g_Main.GetObjectA(0, static_cast<unsigned __int8>(request->byID), request->wIndex))
  {
    _CHRID targetId{};
    targetId.byID = request->byID;
    targetId.wIndex = request->wIndex;
    targetId.dwSerial = request->dwSerial;
    player->pc_ThrowSkillRequest(
      request->wBulletSerial,
      &targetId,
      request->wConsumeItemSerial);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ThrowSkillRequest()..  if(!g_Main.GetObject(obj_kind_char, pRecv->idDst.byID, pRecv->idDst.wIndex))",
    charName);
  return 0;
}

char CNetworkEX::ThrowUnitRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_throw_unit_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  if (g_Main.GetObjectA(0, static_cast<unsigned __int8>(request->byID), request->wIndex))
  {
    _CHRID targetId{};
    targetId.byID = request->byID;
    targetId.wIndex = request->wIndex;
    targetId.dwSerial = request->dwSerial;
    player->pc_ThrowUnitRequest(&targetId, request->wConsumeItemSerial);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ThrowUnitRequest()..  if(!g_Main.GetObject(obj_kind_char, pRecv->idDst.byID, pRecv->idDst.wIndex))",
    charName);
  return 0;
}

char CNetworkEX::MakeTowerRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_make_tower_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const int skillIndex = request->wSkillIndex;
  const int recordNum = g_Main.m_tblEffectData[2].GetRecordNum();
  if (skillIndex < recordNum)
  {
    const unsigned __int8 materialNum = static_cast<unsigned __int8>(request->byMaterialNum);
    if (materialNum <= 0x1Eu)
    {
      for (int index = 0; index < materialNum; ++index)
      {
        const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->Material[index].byMaterSlotIndex);
        const unsigned __int8 amount = static_cast<unsigned __int8>(request->Material[index].byAmount);
        if (slotIndex >= 3u)
        {
          const char *charName = player->m_Param.GetCharNameA();
          m_LogFile.Write(
            "odd.. %s: MakeTowerRequest()..  if(pRecv->Material[i].byMaterSlotIndex >= max_tower_material)",
            charName);
          return 0;
        }
        if (amount > 0x63u)
        {
          const char *charName = player->m_Param.GetCharNameA();
          m_LogFile.Write(
            "odd.. %s: MakeTowerRequest()..  if(pRecv->Material[i].byAmount > max_overlap_num)",
            charName);
          return 0;
        }
      }

      float pos[3]{};
      ShortToFloat(request->zPos, pos, 3);
      player->pc_MakeTowerRequest(
        request->wSkillIndex,
        request->wTowerItemSerial,
        materialNum,
        request->Material,
        pos,
        request->wConsumeItemSerial);
      return 1;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: MakeTowerRequest()..  if(pRecv->byMaterialNum > _make_tower_request_clzo::max_material)",
      charName);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: MakeTowerRequest()..  if(pRecv->wSkillIndex >= g_Main.m_tblEffectData[effect_code_class].GetRecordNum())",
    charName);
  return 0;
}

char CNetworkEX::BackTowerRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_back_tower_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_BackTowerRequest(request->dwTowerObjSerial);
  return 1;
}

char CNetworkEX::MakeTrapRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_make_trap_request_clzo *>(pBuf);
  _STORAGE_LIST::_db_con *itemCon = player->m_Param.m_dbInven.GetPtrFromSerial(request->wTrapItemSerial);
  if (itemCon)
  {
    _base_fld *record = g_Main.m_tblItemData[26].GetRecord(itemCon->m_wItemIndex);
    if (record)
    {
      if (reinterpret_cast<_TrapItem_fld *>(record)->m_bExist == 1)
      {
        CNuclearBombMgr *mgr = CNuclearBombMgr::Instance();
        mgr->SendMsg_Result(n, 1u);
        return 1;
      }

      if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
      {
        return 1;
      }

      const int skillIndex = request->wSkillIndex;
      const int recordNum = g_Main.m_tblEffectData[2].GetRecordNum();
      if (skillIndex < recordNum)
      {
        float pos[3]{};
        ShortToFloat(request->zPos, pos, 3);
        player->pc_MakeTrapRequest(
          request->wSkillIndex,
          request->wTrapItemSerial,
          pos,
          request->wConsumeItemSerial);
        return 1;
      }

      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: MakeTrapRequest()..  if(pRecv->wSkillIndex >= g_Main.m_tblEffectData[effect_code_class].GetRecordNum())",
        charName);
      return 0;
    }
  }

  CNuclearBombMgr *mgr = CNuclearBombMgr::Instance();
  mgr->SendMsg_Result(n, 0xBu);
  return 0;
}

char CNetworkEX::BackTrapRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_back_trap_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_BackTrapRequest(request->dwTrapObjSerial, request->wAddSerial);
  return 1;
}

char CNetworkEX::DecideRecallRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_decide_recall_request_clzo *>(pBuf);
  CRecallEffectController *controller = CRecallEffectController::Instance();
  controller->DecideRecall(
    request->wRequestID,
    static_cast<unsigned __int8>(request->byAgree),
    player);
  return 1;
}

char CNetworkEX::ForceRecallTeleportRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_force_recall_teleport_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  CPlayer *targetPlayer = GetPtrPlayerFromName(g_Player, MAX_PLAYER, request->wszRecallName);
  if (targetPlayer)
  {
    _CHRID targetId{};
    targetId.byID = targetPlayer->m_ObjID.m_byID;
    targetId.dwSerial = targetPlayer->m_ObjID.m_wIndex;
    targetId.wIndex = targetPlayer->m_dwObjSerial;
    player->pc_ForceRequest(
      request->wForceSerial,
      &targetId,
      request->wConsumeItemSerial);
  }
  else
  {
    CRecallEffectController *controller = CRecallEffectController::Instance();
    controller->SendRecallReqeustResult(0x11u, player);
  }
  return 1;
}

char CNetworkEX::MoveToOwnStoneMapRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_MoveToOwnStoneMapRequest();
  }
  return 1;
}

char CNetworkEX::DTradeAskRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_dtrade_ask_request_clzo *>(pBuf);
  CPlayer *targetPlayer = &g_Player[request->wDstIndex];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  if (player->m_EP.GetEff_Have(50) > 0.0)
  {
    player->SendMsg_JadeEffectErr(3u);
    return 1;
  }

  if (player->m_EP.GetEff_State(26) || targetPlayer->m_EP.GetEff_State(26))
  {
    player->SendMsg_DTradeAskResult(0x20u);
    return 1;
  }

  const unsigned __int16 dstIndex = request->wDstIndex;
  if (dstIndex < MAX_PLAYER)
  {
    player->pc_DTradeAskRequest(dstIndex);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: DTradeAskRequest().. if(pRecv->wDstIndex >= MAX_PLAYER)",
    charName);
  return 0;
}

char CNetworkEX::DTradeAnswerRequest(int n, _d_trade_answer_request_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  if (request->idAsker.wIndex < MAX_PLAYER)
  {
    player->pc_DTradeAnswerRequest(&request->idAsker);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: DTradeAnswerRequest() : pRecv->idAsker.wIndex(%d) >= MAX_PLAYER(%d)",
    charName,
    request->idAsker.wIndex,
    MAX_PLAYER);
  return 0;
}

char CNetworkEX::DTradeCancleRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_DTradeCancleRequest();
  }
  return 1;
}

char CNetworkEX::DTradeLockRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_DTradeLockRequest();
  return 1;
}

char CNetworkEX::DTradeAddRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_d_trade_add_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->bySlotIndex);
  if (slotIndex <= 0xFu)
  {
    const unsigned __int8 storageCode = static_cast<unsigned __int8>(request->byStorageCode);
    if (storageCode < 8u)
    {
      player->pc_DTradeAddRequest(
        request->bySlotIndex,
        request->byStorageCode,
        request->dwSerial,
        request->byAmount);
      return 1;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: DTradeAddRequest() : pRecv->byStorageCode(%d) >= _STORAGE_POS::STORAGE_NUM(%d)",
      charName,
      storageCode,
      8);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: DTradeAddRequest() : pRecv->bySlotIndex(%d) > max_d_trade_item(%d)",
    charName,
    slotIndex,
    15);
  return 0;
}

char CNetworkEX::DTradeDelRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_dtrade_del_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->bySlotIndex);
  if (slotIndex <= 0xFu)
  {
    player->pc_DTradeDelRequest(slotIndex);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: DTradeDelRequest().. if(pRecv->bySlotIndex > max_d_trade_item)",
    charName);
  return 0;
}

char CNetworkEX::DTradeBetRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_d_trade_bet_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 moneyUnit = static_cast<unsigned __int8>(request->byMoneyUnit);
  if (moneyUnit == 0 || moneyUnit == 1)
  {
    player->pc_DTradeBetRequest(moneyUnit, request->dwBetAmount);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: DTradeBetRequest().. if(pRecv->byMoneyUnit != money_unit_dalant && pRecv->byMoneyUnit != money_unit_gold)",
    charName);
  return 0;
}

char CNetworkEX::DTradeOKRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_dtrade_ok_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_DTradeOKRequest(request->dwKey);
  return 1;
}

char CNetworkEX::AnimusRecallRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_animus_recall_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_AnimusRecallRequest(
    request->wAnimusItemSerial,
    request->wAnimusClientHP,
    request->wAnimusClientFP);
  return 1;
}

char CNetworkEX::AnimusReturnRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_AnimusReturnRequest();
  return 1;
}

char CNetworkEX::AnimusCommandRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_animus_command_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 commandCode = static_cast<unsigned __int8>(request->byCommandCode);
  if (commandCode < 2u)
  {
    player->pc_AnimusCommandRequest(commandCode);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: AnimusCommandRequest()..  if(pRecv->byCommandCode >= ANIMUS_ACT_TYPE_NUM)",
    charName);
  return 0;
}

char CNetworkEX::AnimusTargetRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_animus_target_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_bCorpse)
  {
    return 1;
  }

  if (g_Main.GetObjectA(
        0,
        static_cast<unsigned __int8>(request->byObjectID),
        request->wObjectIndex))
  {
    player->pc_AnimusTargetRequest(
      static_cast<unsigned __int8>(request->byObjectID),
      request->wObjectIndex,
      request->dwObjectSerial);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: AnimusTargetRequest()..  if(!g_Main.GetObject(obj_kind_char, pRecv->byObjectID, pRecv->wObjectIndex))",
    charName);
  return 0;
}

char CNetworkEX::UnitFrameBuyRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_unit_frame_buy_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 frameCode = static_cast<unsigned __int8>(request->byFrameCode);
  if (frameCode < 4u)
  {
    player->pc_UnitFrameBuyRequest(frameCode, request->bUseNPCLinkIntem);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: UnitFrameBuyRequest().. if(pRecv->byFrameCode >= UNIT_FRAME_CODE_NUM)",
    charName);
  return 0;
}

char CNetworkEX::UnitSellRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_unit_sell_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->bySlotIndex);
  if (slotIndex < 4u)
  {
    player->pc_UnitSellRequest(slotIndex, request->bUseNPCLinkIntem);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: UnitSellRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
    charName);
  return 0;
}

char CNetworkEX::UnitPartTuningRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_unit_part_tuning_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->bySlotIndex);
  if (slotIndex < 4u)
  {
    const unsigned __int8 tuningNum = static_cast<unsigned __int8>(request->byTuningNum);
    if (tuningNum <= 6u)
    {
      for (int index = 0; index < tuningNum; ++index)
      {
        const unsigned __int8 partCode = static_cast<unsigned __int8>(request->TuningList[index].byPartCode);
        if (partCode >= 6u)
        {
          const char *charName = player->m_Param.GetCharNameA();
          m_LogFile.Write(
            "odd.. %s: UnitPartTuningRequest().. if(pRecv->TuningList[i].byPartCode >= UNIT_PART_NUM)",
            charName);
          return 0;
        }
      }

      player->pc_UnitPartTuningRequest(
        slotIndex,
        tuningNum,
        request->TuningList,
        request->bUseNPCLinkIntem);
      return 1;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: UnitPartTuningRequest().. if(pRecv->byTuningNum > UNIT_PART_NUM)",
      charName);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: UnitPartTuningRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
    charName);
  return 0;
}

char CNetworkEX::UnitFrameRepairRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_unit_frame_repair_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->bySlotIndex);
  if (slotIndex < 4u)
  {
    player->pc_UnitFrameRepairRequest(slotIndex, request->bUseNPCLinkIntem);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: UnitFrameRepairRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
    charName);
  return 0;
}

char CNetworkEX::UnitBulletFillRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_unit_bullet_fill_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->bySlotIndex);
  if (slotIndex < 4u)
  {
    unsigned __int8 fillMask = 0;
    for (int index = 0; index < 2; ++index)
    {
      const unsigned __int16 bulletIndex = request->wBulletIndex[index];
      if (bulletIndex != 0xFFFF)
      {
        if (!g_Main.m_tblUnitBullet.GetRecord(bulletIndex))
        {
          const char *charName = player->m_Param.GetCharNameA();
          m_LogFile.Write(
            "odd.. %s: UnitBulletFillRequest().. if(!g_Main.m_tblUnitBullet.GetRecord(pRecv->wBulletIndex[i]))",
            charName);
          return 0;
        }
        fillMask |= static_cast<unsigned __int8>(1 << index);
      }
    }

    if (fillMask)
    {
      player->pc_UnitBulletFillRequest(slotIndex, request->wBulletIndex, request->bUseNPCLinkIntem);
      return 1;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: UnitBulletFillRequest().. if(!byFill)", charName);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: UnitBulletFillRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
    charName);
  return 0;
}

char CNetworkEX::UnitPackFillRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_unit_pack_fill_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->bySlotIndex);
  if (slotIndex < 4u)
  {
    const unsigned __int8 fillNum = static_cast<unsigned __int8>(request->byFillNum);
    if (fillNum <= 8u)
    {
      unsigned __int8 spareUsed[8]{};
      for (int index = 0; index < fillNum; ++index)
      {
        const unsigned __int8 spareIndex = static_cast<unsigned __int8>(request->List[index].bySpareIndex);
        const unsigned __int16 bulletIndex = request->List[index].wBulletIndex;
        if (spareIndex >= 8u)
        {
          const char *charName = player->m_Param.GetCharNameA();
          m_LogFile.Write(
            "odd.. %s: UnitBulletFillRequest().. if(pRecv->List[i].bySpareIndex >= max_unit_spare)",
            charName);
          return 0;
        }
        if (!g_Main.m_tblUnitBullet.GetRecord(bulletIndex))
        {
          const char *charName = player->m_Param.GetCharNameA();
          m_LogFile.Write(
            "odd.. %s: UnitBulletFillRequest().. if(pRecv->List[i].bySpareIndex >= max_unit_spare)",
            charName);
          return 0;
        }
        if (spareUsed[spareIndex])
        {
          const char *charName = player->m_Param.GetCharNameA();
          m_LogFile.Write(
            "odd.. %s: UnitBulletFillRequest().. if(bSpare[pRecv->List[i].bySpareIndex])",
            charName);
          return 0;
        }
        spareUsed[spareIndex] = 1;
      }

      player->pc_UnitPackFillRequest(
        slotIndex,
        fillNum,
        request->List,
        request->bUseNPCLinkIntem);
      return 1;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: UnitBulletFillRequest().. if(pRecv->byFillNum > max_unit_spare)",
      charName);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: UnitBulletFillRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
    charName);
  return 0;
}

char CNetworkEX::UnitDeliveryRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_unit_delivery_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->bySlotIndex);
  if (slotIndex >= 4u)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: UnitDeliveryRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
      charName);
    return 0;
  }

  CItemStore *store = nullptr;
  if (request->dwStoreIndex != 0xFFFFFFFF)
  {
    const int mapCode = player->m_pCurMap->GetMapCode();
    CItemStoreManager *manager = CItemStoreManager::Instance();
    CMapItemStoreList *storeList = manager->GetMapItemStoreListBySerial(mapCode);
    if (!storeList)
    {
      return 1;
    }

    for (int index = 0; index < storeList->m_nItemStoreNum; ++index)
    {
      CItemStore *entry = &storeList->m_ItemStore[index];
      if (entry->m_pRec->m_dwIndex == request->dwStoreIndex)
      {
        store = entry;
        break;
      }
    }

    if (!store)
    {
      return 1;
    }
  }

  float pos[3]{};
  ShortToFloat(request->zUnitNewPos, pos, 3);
  player->pc_UnitDeliveryRequest(
    slotIndex,
    store,
    1,
    pos,
    request->bUseNPCLinkIntem);
  return 1;
}

char CNetworkEX::UnitReturnRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_UnitReturnRequest();
  return 1;
}

char CNetworkEX::UnitTakeRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_UnitTakeRequest();
  return 1;
}

char CNetworkEX::UnitLeaveRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_unit_leave_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  float pos[3]{};
  ShortToFloat(request->zPos, pos, 3);
  player->pc_UnitLeaveRequest(pos);
  return 1;
}

char CNetworkEX::UnitBulletReplaceRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_unit_bullet_replace_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->bySlotIndex);
  if (slotIndex < 4u)
  {
    const unsigned __int8 packIndex = static_cast<unsigned __int8>(request->byPackIndex);
    if (packIndex < 8u)
    {
      const unsigned __int8 bulletPart = static_cast<unsigned __int8>(request->byBulletPart);
      if (bulletPart < 2u)
      {
        player->pc_UnitBulletReplaceRequest(slotIndex, packIndex, bulletPart);
        return 1;
      }

      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: UnitBulletReplaceRequest().. if(pRecv->byBulletPart >= UNIT_BULLET_NUM)",
        charName);
      return 0;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: UnitBulletReplaceRequest().. if(pRecv->byPackIndex >= max_unit_spare)",
      charName);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: UnitBulletReplaceRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
    charName);
  return 0;
}

char CNetworkEX::SelectWaitedQuestReport(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_select_waited_quest_report_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  const unsigned __int8 selectIndex = static_cast<unsigned __int8>(request->bySelectRewardItem);
  if (selectIndex < 5u)
  {
    player->pc_SelectQuestAfterHappenEvent(selectIndex);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: SelectClassRequest()..  if(pRecv->bySelectRewardItem >= max_class_bns_item)",
    charName);
  return 0;
}

char CNetworkEX::QuestSelectRewardReport(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_quest_select_reward_report_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  const unsigned __int8 questSlot = static_cast<unsigned __int8>(request->byQuestSlot);
  if (questSlot < 0x1Eu)
  {
    const unsigned __int8 rewardSlot = static_cast<unsigned __int8>(request->bySelectItemSlotIndex);
    const unsigned __int8 linkIndex = static_cast<unsigned __int8>(request->byLinkQuestIndex);
    if (rewardSlot == 0xFF || rewardSlot < 6u)
    {
      if (linkIndex == 0xFF || linkIndex < 5u)
      {
        if (rewardSlot == 0xFF && linkIndex == 0xFF)
        {
          const char *charName = player->m_Param.GetCharNameA();
          m_LogFile.Write(
            "odd.. %s: QuestSelectRewardReport()..  if(pRecv->bySelectItemSlotIndex == 0xFF && pRecv->byLinkQuestIndex == 0xFF)",
            charName);
          return 0;
        }

        player->pc_SelectQuestReward(questSlot, rewardSlot, linkIndex);
        return 1;
      }

      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: QuestSelectRewardReport()..  if(pRecv->byLinkQuestIndex >= link_quest_num)",
        charName);
      return 0;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: QuestSelectRewardReport()..  if(pRecv->bySelectItemSlotIndex >= reward_item_num)",
      charName);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: QuestSelectRewardReport()..  if(pRecv->byQuestDBSlot >= quest_storage_num)",
    charName);
  return 0;
}

char CNetworkEX::NPCDialogRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_npc_rec_index_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  CItemStoreManager *storeMgr = CItemStoreManager::Instance();
  const unsigned int recordNum = storeMgr->m_tblItemStore.GetRecordNum();
  const unsigned int recIndex = request->dwRecIndex;
  if (recIndex >= recordNum)
  {
    return 1;
  }

  const int mapCode = player->m_pCurMap->GetMapCode();
  CMapItemStoreList *storeList = storeMgr->GetMapItemStoreListBySerial(mapCode);
  if (!storeList)
  {
    return 1;
  }

  CItemStore *store = storeList->GetItemStoreFromRecIndex(recIndex);
  if (store)
  {
    player->pc_RequestDialogWithNPC(store);
  }
  return 1;
}

char CNetworkEX::NPCWatchingRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_npc_rec_index_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  CItemStoreManager *storeMgr = CItemStoreManager::Instance();
  const unsigned int recordNum = storeMgr->m_tblItemStore.GetRecordNum();
  const unsigned int recIndex = request->dwRecIndex;
  if (recIndex >= recordNum)
  {
    return 1;
  }

  const int mapCode = player->m_pCurMap->GetMapCode();
  CMapItemStoreList *storeList = storeMgr->GetMapItemStoreListBySerial(mapCode);
  if (!storeList)
  {
    return 1;
  }

  CItemStore *store = storeList->GetItemStoreFromRecIndex(recIndex);
  if (store)
  {
    player->pc_RequestWatchingWithNPC(store);
  }
  return 1;
}

char CNetworkEX::NPCQuestListRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_npc_rec_index_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  CItemStoreManager *storeMgr = CItemStoreManager::Instance();
  const unsigned int recordNum = storeMgr->m_tblItemStore.GetRecordNum();
  const unsigned int recIndex = request->dwRecIndex;
  if (recIndex >= recordNum)
  {
    return 1;
  }

  const int mapCode = player->m_pCurMap->GetMapCode();
  CMapItemStoreList *storeList = storeMgr->GetMapItemStoreListBySerial(mapCode);
  if (!storeList)
  {
    return 1;
  }

  CItemStore *store = storeList->GetItemStoreFromRecIndex(recIndex);
  if (store)
  {
    player->pc_RequestQuestListFromNPC(store);
  }
  return 1;
}

char CNetworkEX::NPCQuestRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_npc_quest_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  CItemStoreManager *storeMgr = CItemStoreManager::Instance();
  const unsigned int recordNum = storeMgr->m_tblItemStore.GetRecordNum();
  const unsigned int recIndex = request->dwRecIndex;
  if (recIndex >= recordNum)
  {
    return 1;
  }

  const int mapCode = player->m_pCurMap->GetMapCode();
  CMapItemStoreList *storeList = storeMgr->GetMapItemStoreListBySerial(mapCode);
  if (!storeList)
  {
    return 1;
  }

  CItemStore *store = storeList->GetItemStoreFromRecIndex(recIndex);
  if (store)
  {
    player->pc_RequestQuestFromNPC(store, request->dwNPCQuestIndex);
  }
  return 1;
}

char CNetworkEX::QuestGiveupRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_quest_giveup_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->byQuestDBSlot);
  if (slotIndex < 0x1Eu)
  {
    player->pc_QuestGiveupRequest(slotIndex);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: QuestGiveupRequest()..  if(pRecv->byQuestDBSlot >= quest_storage_num)",
    charName);
  return 0;
}

char CNetworkEX::BriefPassReport(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_brief_pass_report_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 slotIndex = static_cast<unsigned __int8>(request->bySlotIndex);
  if (slotIndex < 0x1Eu)
  {
    player->pc_BriefPass(slotIndex);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: BriefPassReport()..  if(pRecv->bySlotIndex >= quest_storage_num)",
    charName);
  return 0;
}

char CNetworkEX::CastVoteRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_cast_vote_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 code = static_cast<unsigned __int8>(request->byCode);
  if (code <= 2u)
  {
    player->pc_CastVoteRequest(request->nVoteSerial, code);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write("odd.. %s: CastVoteRequest()..  if(pRecv->byCode > 2)", charName);
  return 0;
}

char CNetworkEX::ProposeVoteRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_propose_vote_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int16 contentSize = request->wContentSize;
  if (contentSize <= 0x4FFu)
  {
    if (contentSize != 0)
    {
      const unsigned __int8 limGrade = static_cast<unsigned __int8>(request->byLimGrade);
      if (limGrade <= 7u)
      {
        char content[1296];
        memcpy_0(content, request->wszContent, contentSize);
        content[contentSize] = 0;
        player->pc_ProposeVoteRequest(limGrade, content);
        return 1;
      }

      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write("odd.. %s: ProposeVoteRequest()..  if(pRecv->byLimGrade > max_grade)", charName);
      return 0;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: ProposeVoteRequest()..  if(pRecv->byContentSize == 0)", charName);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ProposeVoteRequest()..  if(pRecv->byContentSize > _propose_vote_request_clzo::content_size-1)",
    charName);
  return 0;
}

char CNetworkEX::ObjectServerPosRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_object_server_pos_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }
  if (!player->m_byUserDgr)
  {
    return 1;
  }

  unsigned __int8 resultCode = 0;
  CGameObject *target = g_Main.GetObjectA(
    static_cast<unsigned __int8>(request->byObjKind),
    static_cast<unsigned __int8>(request->byID),
    request->wIndex);

  if (target)
  {
    if (target->m_bLive)
    {
      if (target->m_dwObjSerial != request->dwObjSerial)
      {
        resultCode = 3;
      }
    }
    else
    {
      resultCode = 2;
    }
  }
  else
  {
    resultCode = 1;
  }

  _object_server_pos_result_zocl msg{};
  msg.byErrCode = static_cast<char>(resultCode);
  msg.byObjKind = request->byObjKind;
  msg.byObjID = request->byID;
  msg.wObjIndex = request->wIndex;
  msg.dwObjSerial = request->dwObjSerial;
  if (!resultCode && target)
  {
    memcpy_0(msg.fServerPos, target->m_fCurPos, sizeof(msg.fServerPos));
  }

  unsigned __int8 type[2]{20, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
  return 1;
}

char CNetworkEX::GuildEstablishRequest(int n, const _guild_establish_request_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  char guildName[144];
  memcpy_0(guildName, request->wszGuildName, sizeof(request->wszGuildName));
  guildName[16] = 0;
  player->pc_GuildEstablishRequest(guildName);
  return 1;
}

char CNetworkEX::GuildDownloadRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return 1;
  }

  if (player->m_bGuildDownload)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write("odd.. %s: GuildDownloadRequest()..  if(pOne->m_bGuildDownload)", charName);
    return 0;
  }

  player->pc_GuildDownLoadRequest();
  return 1;
}

char CNetworkEX::GuildJoinApplyRequest(int n, const _guild_join_apply_request_clzo *request)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    char guildName[144];
    memcpy_0(guildName, request->wszGuildName, sizeof(request->wszGuildName));
    guildName[16] = 0;
    player->pc_GuildJoinApplyRequest(guildName);
  }
  return 1;
}

char CNetworkEX::GuildJoinApplyCancelRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_GuildJoinApplyCancelRequest();
  }
  return 1;
}

char CNetworkEX::GuildJoinAcceptRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_guild_join_accept_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    player->pc_GuildJoinAcceptRequest(
      request->dwApplierSerial,
      static_cast<unsigned __int8>(request->bAccept));
  }
  return 1;
}

char CNetworkEX::GuildSelfLeaveRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_GuildSelfLeaveRequest();
  }
  return 1;
}

char CNetworkEX::GuildOfferSuggestRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_guild_offer_suggest_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  if (request->byMatterType != 6)
  {
    return 0;
  }

  const unsigned __int8 commentSize = static_cast<unsigned __int8>(request->byCommentLen);
  if (commentSize >= 0x40u)
  {
    return 0;
  }

  char comment[96];
  memcpy_0(comment, request->wszComment, commentSize);
  comment[commentSize] = 0;
  player->pc_GuildOfferSuggestRequest(
    request->byMatterType,
    request->dwMatterDst,
    comment,
    request->dwMatterObj1,
    request->dwMatterObj2,
    request->dwMatterObj3);
  return 1;
}

char CNetworkEX::GuildCancelSuggestRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_guild_cancel_suggest_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_GuildCancelSuggestRequest(request->dwMatterVoteSynKey);
  }
  return 1;
}

char CNetworkEX::GuildVoteRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_guild_vote_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 voteType = static_cast<unsigned __int8>(request->byVoteCode);
  if (voteType >= 2u)
  {
    return 0;
  }

  player->pc_GuildVoteRequest(request->dwMatterVoteSynKey, voteType);
  return 1;
}

char CNetworkEX::GuildQueryInfoRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_guild_query_info_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_GuildQueryInfoRequest(request->dwGuildSerial);
  }
  return 1;
}

char CNetworkEX::GuildPushMoneyRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_guild_push_money_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_GuildPushMoneyRequest(
      request->dwPushDalant,
      request->dwPushGold);
  }
  return 1;
}

char CNetworkEX::GuildManageRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_guild_manage_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 manageType = static_cast<unsigned __int8>(request->byManageType);
  if (manageType < 6u)
  {
    player->pc_GuildManageRequest(
      manageType,
      request->dwManageDst,
      request->dwManageObj1,
      request->dwManageObj2,
      request->dwManageObj3);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: GuildManageRequest()..  if (pRecv->byManageType < 0 || pRecv->byManageType >= guild_manage_type_num)",
    charName);
  return 0;
}

char CNetworkEX::GuildBattlePossibleGuildBattleList(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_possible_battle_guild_list_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    const int raceCode = player->m_Param.GetRaceCode();
    CGuildBattleController *controller = CGuildBattleController::Instance();
    controller->SendPossibleBattleGuildList(
      n,
      raceCode,
      request->byPage,
      request->dwCurVer);
  }
  return 1;
}

char CNetworkEX::GuildBattleRankListRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_guild_battle_rank_list_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  unsigned int guildSerial = static_cast<unsigned int>(-1);
  if (player->m_Param.m_pGuild)
  {
    guildSerial = player->m_Param.m_pGuild->m_dwSerial;
  }
  const unsigned int mapId = static_cast<unsigned __int8>(request->byRace);
  const int raceCode = player->m_Param.GetRaceCode();
  CGuildBattleController *controller = CGuildBattleController::Instance();
  controller->SendRankList(
    n,
    raceCode,
    request->dwCurVer,
    mapId,
    request->byPage,
    guildSerial);
  return 1;
}

char CNetworkEX::GuildBattleJoinGuildBattleRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    CGuildBattleController *controller = CGuildBattleController::Instance();
    controller->Join(player);
  }
  return 1;
}

char CNetworkEX::GuildBattleReservedScheduleRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_guild_battle_reserved_schedule_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  unsigned int guildSerial = static_cast<unsigned int>(-1);
  if (player->m_Param.m_pGuild)
  {
    guildSerial = player->m_Param.m_pGuild->m_dwSerial;
  }
  const unsigned int mapId = player->m_Param.GetRaceCode();
  CGuildBattleController *controller = CGuildBattleController::Instance();
  controller->SendReservedScheduleList(
    n,
    mapId,
    request->dwVer,
    request->byDay,
    request->byPage,
    guildSerial);
  return 1;
}

char CNetworkEX::GuildBattleCurrentBattleInfoRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    const unsigned int mapId = player->m_Param.GetRaceCode();
    CGuildBattleController *controller = CGuildBattleController::Instance();
    controller->SendCurrentBattleInfoRequest(n, mapId);
  }
  return 1;
}

char CNetworkEX::GuildBattleTakeGravityStoneRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_guild_battle_take_gravity_stone_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    CGuildBattleController *controller = CGuildBattleController::Instance();
    controller->CheckTakeGravityStone(request->nObjectIndex, player);
  }
  return 1;
}

char CNetworkEX::GuildBattleGetGravityStoneRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_guild_battle_get_gravity_stone_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    CGuildBattleController *controller = CGuildBattleController::Instance();
    controller->CheckGetGravityStone(request->wIndex, request->dwObjSerial, player);
  }
  return 1;
}

char CNetworkEX::GuildBattleGoalRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_guild_battle_goal_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    CGuildBattleController *controller = CGuildBattleController::Instance();
    controller->CheckGoal(player, request->nGoalObjectIndex);
  }
  return 1;
}

char CNetworkEX::GuildRoomRentRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_GuildRoomRentRequest(reinterpret_cast<_guildroom_rent_request_clzo *>(pBuf));
  }
  return 1;
}

char CNetworkEX::GuildRoomEnterRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_GuildRoomEnterRequest(reinterpret_cast<_guildroom_enter_request_clzo *>(pBuf));
  }
  return 1;
}

char CNetworkEX::GuildRoomOutRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_GuildRoomOutRequest(reinterpret_cast<_guildroom_out_request_clzo *>(pBuf));
  }
  return 1;
}

char CNetworkEX::GuildRoomRestTimeRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    // this is not a stub
  }
  return 1;
}

char CNetworkEX::GuildHonorListRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_guild_honor_list_request_clzo *>(pBuf);
  if (player->m_bLoad)
  {
    player->pc_GuildHonorListRequest(request->byDivision);
  }
  return 1;
}

char CNetworkEX::GuildSetHonorRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }

  _guild_honor_set_request_clzo *data = reinterpret_cast<_guild_honor_set_request_clzo *>(pBuf);
  if (data->byListNum <= 5u)
  {
    player->pc_GuildSetHonorRequest(data);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: GuildSetHonorRequest()..  f (pRecv->byListNum <= 0 || pRecv->byListNum > 5)",
    charName);
  return 0;
}

char CNetworkEX::GuildListRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_guild_list_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int8 page = static_cast<unsigned __int8>(request->byPage);
  if (page < 0x4Bu)
  {
    player->pc_GuildListRequest(page);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: GuildListRequest()..  if (pRecv->byPage < 0 || pRecv->byPage >= 75)",
    charName);
  return 0;
}

char CNetworkEX::GuildNextHonorListRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_GuildNextHonorListRequest();
  }
  return 1;
}

char CNetworkEX::ATradeRegItemRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  _a_trade_reg_item_request_clzo *request = reinterpret_cast<_a_trade_reg_item_request_clzo *>(pBuf);
  if (request->byItemTableCode < 0x25u)
  {
    const int recordNum = g_Main.m_tblItemData[request->byItemTableCode].GetRecordNum();
    if (request->wItemIndex < recordNum)
    {
      if (request->byAmount <= 0x63u)
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->Regist(static_cast<unsigned __int16>(n), request);
        return 1;
      }

      const char *charName = player->m_Param.GetCharNameA();
      m_LogFile.Write(
        "odd.. %s: ATradeRegItemRequest().. if(pRecv->byAmount > max_overlap_num)",
        charName);
      return 0;
    }

    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: ATradeRegItemRequest().. if(pRecv->wItemIndex >= g_Main.m_tblItemData[pRecv->byItemTableCode].GetRecordNum())",
      charName);
    return 0;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ATradeRegItemRequest().. if(pRecv->byItemTableCode >= item_tbl_num)",
    charName);
  return 0;
}

char CNetworkEX::ATradeAdjustPriceRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  _a_trade_adjust_price_request_clzo *request = reinterpret_cast<_a_trade_adjust_price_request_clzo *>(pBuf);
  CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
  controller->ModifyPrice(static_cast<unsigned __int16>(n), request);
  return 1;
}

char CNetworkEX::ATradeClearItemRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  _a_trade_clear_item_request_clzo *request = reinterpret_cast<_a_trade_clear_item_request_clzo *>(pBuf);
  CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
  controller->CancelRegist(static_cast<unsigned __int16>(n), request);
  return 1;
}

char CNetworkEX::ATradeTaxRateRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  const int raceCode = player->m_Param.GetRaceCode();
  CUnmannedTraderTaxRateManager *taxMgr = CUnmannedTraderTaxRateManager::Instance();
  taxMgr->SendTaxRate(n, raceCode);
  return 1;
}

char CNetworkEX::ATradeBuyItemRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  _unmannedtrader_buy_item_request_clzo *request = reinterpret_cast<_unmannedtrader_buy_item_request_clzo *>(pBuf);
  if (request->byNum <= 0xAu)
  {
    CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
    controller->Buy(static_cast<unsigned __int16>(n), request);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: ATradeBuyItemRequest().. if(pRecv->byNum > _unmannedtrader_buy_item_request_clzo::max_list)",
    charName);
  return 0;
}

char CNetworkEX::ATradeRegedListRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  _unmannedtrader_search_list_request_clzo *request = reinterpret_cast<_unmannedtrader_search_list_request_clzo *>(pBuf);
  CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
  controller->Search(static_cast<unsigned __int16>(n), request);
  return 1;
}

char CNetworkEX::ATradeReRegistRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return 1;
  }

  _unmannedtrader_re_regist_request_clzo *request = reinterpret_cast<_unmannedtrader_re_regist_request_clzo *>(pBuf);
  CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
  controller->ReRegist(static_cast<unsigned __int16>(n), request);
  return 1;
}

char CNetworkEX::BuddyDownloadRequest(int n, char *pBuf)
{
CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return 1;
  }
  if (player->m_bBuddyListDownload)
  {
    return 0;
  }
  player->pc_BuddyDownloadRequest();
  return 1;
}

char CNetworkEX::BuddyAddRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_buddy_add_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  char dstName[40];
  memset(dstName, 0, 17);
  const unsigned __int16 dstIndex = request->wDstIndex;
  if (dstIndex == 0xFFFF)
  {
    memcpy_0(dstName, request->wszDstName, 0x10uLL);
    dstName[16] = 0;
  }
  else if (dstIndex >= MAX_PLAYER)
  {
    const char *charName = player->m_Param.GetCharNameA();
    m_LogFile.Write(
      "odd.. %s: BuddyAddRequest()..  if(pRecv->wDstIndex >= MAX_PLAYER)",
      charName);
    return 0;
  }

  player->pc_BuddyAddRequest(
    dstIndex,
    request->dwDstSerial,
    dstName);
  return 1;
}

char CNetworkEX::BuddyAddAnswer(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *request = reinterpret_cast<_buddy_add_answer_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return 1;
  }

  const unsigned __int16 askerIndex = request->wAskerIndex;
  if (askerIndex < MAX_PLAYER)
  {
    player->pc_BuddyAddAnswer(
      static_cast<unsigned __int8>(request->bAccept),
      askerIndex,
      request->dwAskerSerial);
    return 1;
  }

  const char *charName = player->m_Param.GetCharNameA();
  m_LogFile.Write(
    "odd.. %s: BuddyAddAnswer()..  if(pRecv->wAskerIndex >= MAX_PLAYER)",
    charName);
  return 0;
}

char CNetworkEX::BuddyDelRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_buddy_del_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_BuddyDelRequest(request->dwBuddySerial);
  }
  return 1;
}

char CNetworkEX::ExchangeGoldForPvPRequest(int n, char *pBuf)
{
  auto *request = reinterpret_cast<_exchange_gold_for_pvp_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  player->pc_ExchangeGoldForPvP(request->dwGold);
  return 1;
}

char CNetworkEX::OpenControlInform(unsigned int n, const _open_control_inform_acwr *request)
{
  (void)n;
  (void)request;
  return 1; // this is not a stub
}

char CNetworkEX::UserBlockResult(unsigned int n, const _user_block_result_acwr *request)
{
  (void)n;
  _CLID targetId = request->idLocalForTarget;
  _CLID gmId = request->idLocalForGM;
  g_Main.pc_UserChatBlockResult(
    request->byBlockResult,
    &targetId,
    &gmId,
    request->bLogin);
  return 1;
}

