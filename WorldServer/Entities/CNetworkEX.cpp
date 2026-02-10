#include "pch.h"

#include "CNetworkEX.h"

#include <cstdlib>
#include <ctime>
#include <cstring>

#include "AutoMineMachineMng.h"
#include "AutominePersonalMgr.h"
#include "CashItemRemoteStore.h"
#include "FireGuard.h"
#include "CBillingManager.h"
#include "CCashDBWorkManager.h"
#include "CLogTypeDBTaskManager.h"
#include "CMainThread.h"
#include "CNationSettingManager.h"
#include "CNuclearBombMgr.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CGuild.h"
#include "CheatCommands.h"
#include "PatriarchElectProcessor.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "CRaceBossMsgController.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "cancel_raceboss_msg_result_zoct.h"
#include "character_disconnect_result_wrac.h"
#include "connection_status_result_zoct.h"
#include "enter_world_request_zone.h"
#include "enter_world_result_zone.h"
#include "notify_local_time_result_zocl.h"
#include "other_shape_request_clzo.h"
#include "server_notify_inform_zone.h"
#include "w_name.h"

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
          result = EnterWorldRequest(n, pMsgHeader, pMsg);
          break;
        case 5:
          result = MoveLobbyRequest(n, pMsg);
          break;
        case 7:
          result = RegedCharRequest(n, pMsg);
          break;
        case 9:
          result = AddCharRequest(n, pMsg);
          break;
        case 11:
          result = DelCharRequest(n, pMsg);
          break;
        case 13:
          result = SelCharRequest(n, pMsg);
          break;
        case 16:
          result = TutorialProcessReport(n, pMsg);
          break;
        case 21:
          result = AliveCharRequest(n, pMsg);
          break;
        case 25:
          result = NotifyLocalTimeRequest(n, pMsg);
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
          result = ChatOperatorRequest(n, pMsg);
          break;
        case 1:
          result = ChatCircleRequest(n, pMsg);
          break;
        case 2:
          result = ChatFarRequest(n, pMsg);
          break;
        case 4:
          result = ChatPartyRequest(n, pMsg);
          break;
        case 5:
          result = ChatRaceRequest(n, pMsg);
          break;
        case 6:
          result = ChatCheatRequest(n, pMsg);
          break;
        case 7:
          result = ChatManageRequest(n, pMsg);
          break;
        case 8:
          result = ChatMgrWhisperRequest(n, pMsg);
          break;
        case 9:
          result = ChatMapRecvYesOrNo(n, pMsg);
          break;
        case 10:
          result = ChatMapRequest(n, pMsg);
          break;
        case 11:
          result = ChatRaceBossRequest(n, pMsg);
          break;
        case 12:
          result = ChatGuildEstSenRequest(n, pMsg);
          break;
        case 13:
          result = ChatRePresentationRequest(n, pMsg);
          break;
        case 14:
          result = ChatAllRecvYesOrNo(n, pMsg);
          break;
        case 15:
          result = ChatAllRequest(n, pMsg);
          break;
        case 16:
          result = ChatGreetingMsg_GM(n, pMsg);
          break;
        case 17:
          result = ChatGreetingMsg_RACE(n, pMsg);
          break;
        case 18:
          result = ChatGreetingMsg_GUILD(n, pMsg);
          break;
        case 19:
          result = ChatTradeRequestMsg(n, pMsg);
          break;
        case 98:
          result = ChatGuildRequest(n, pMsg);
          break;
        case 100:
          result = ChatMultiFarRequest(n, pMsg);
          break;
        case 103:
          result = ChatRaceBossCryRequest(n, pMsg);
          break;
        case 104:
          result = ChatGmNoticeRequest(n, pMsg);
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
          result = NewPosStartRequest(n, pMsg);
          break;
        case 2:
          result = BaseDownloadRequest(n, pMsg);
          break;
        case 4:
          result = InvenDownloadRequest(n, pMsg);
          break;
        case 6:
          result = CumDownloadRequest(n, pMsg);
          break;
        case 8:
          result = ForceDownloadRequest(n, pMsg);
          break;
        case 10:
          result = QuestDownloadRequest(n, pMsg);
          break;
        case 12:
          result = SpecialDownloadRequest(n, pMsg);
          break;
        case 29:
          result = OtherShapeRequest(n, pMsg);
          break;
        case 36:
          result = Revival(n, pMsg);
          break;
        case 40:
          result = ExitWorldRequest(n, pMsg);
          break;
        case 45:
          result = LinkBoardDownloadRequest(n, pMsg);
          break;
        case 48:
          result = MacroDownLoadRequest(n, pMsg);
          break;
        case 53:
          result = AMP_DownloadRequest(n, pMsg);
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
          result = NextPoint(n, pMsg);
          break;
        case 1:
          result = RealMovPosRequest(n, pMsg);
          break;
        case 18:
          result = Stop(n, pMsg);
          break;
        case 25:
          result = GotoBasePortalRequest(n, pMsg);
          break;
        case 27:
          result = GotoAvatorRequest(n, pMsg);
          break;
        case 30:
          result = MoveTypeChangeRequeset(n, pMsg);
          break;
        case 31:
          result = MoveInfoRequeset(n, pMsg);
          break;
        case 32:
          result = PlayerInfoResult(n, pMsg);
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
          result = AttackPersonalRequest(n, pMsg);
          break;
        case 1:
          result = AttackSkillRequest(n, pMsg);
          break;
        case 2:
          result = AttackForceRequest(n, pMsg);
          break;
        case 3:
          result = AttackUnitRequest(n, pMsg);
          break;
        case 4:
          result = AttackTestRequest(n, pMsg);
          break;
        case 54:
          result = AttackSiegeRequest(n, pMsg);
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
          result = ItemboxTakeRequest(n, pMsg);
          break;
        case 3:
          result = ThrowStorageRequest(n, pMsg);
          break;
        case 5:
          result = UsePotionRequest(n, pMsg);
          break;
        case 7:
          result = EquipPartRequest(n, pMsg);
          break;
        case 9:
          result = EmbellishRequest(n, pMsg);
          break;
        case 11:
          result = OffPartRequest(n, pMsg);
          break;
        case 13:
          result = MakeItemRequest(n, pMsg);
          break;
        case 15:
          result = UpgradeItemRequest(n, pMsg);
          break;
        case 17:
          result = DownGradeItemRequest(n, pMsg);
          break;
        case 19:
          result = AddBagRequest(n, pMsg);
          break;
        case 24:
          result = UseRecoverLossExpItemRequest(n, pMsg);
          break;
        case 26:
          result = CombineItemRequest(n, pMsg);
          break;
        case 28:
          result = ExchangeItemRequest(n, pMsg);
          break;
        case 30:
          result = CombineExItemRequest(n, pMsg);
          break;
        case 33:
          result = CombineExItemAcceptRequest(n, pMsg);
          break;
        case 35:
          result = UseFireCrackerItemRequest(n, pMsg);
          break;
        case 43:
          result = SetItemCheckRequest(n, pMsg);
          break;
        case 44:
          result = UseSoccerBallItemRequest(n, pMsg);
          break;
        case 47:
          result = UseRadarItemRequest(n, pMsg);
          break;
        case 49:
          result = RadarCharListRequest(n, pMsg);
          break;
        case 57:
          result = NPCLinkCheckItemRequest(n, pMsg);
          break;
        case 61:
          result = UseRecallTeleportItemRequest(n, pMsg);
          break;
        case 64:
          result = CharacterRenameCash(n, pMsg);
          break;
        case 73:
          result = TalikCrystalExchangeRequest(n, pMsgHeader, pMsg);
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
        result = MovePortalRequest(n, pMsg);
      }
      else if (subType == 3)
      {
        result = RegistBindRequest(n, pMsg);
      }
      else
      {
        result = subType == 9 && EnterReturnGateRequest(n, pMsg);
      }
      break;
    }
    case 10:
    {
      const unsigned __int8 subType = pMsgHeader->m_byType[1];
      switch (subType)
      {
        case 6u:
          result = SelectClassRequest(n, pMsg);
          break;
        case 0x18u:
          result = InitClassRequest(n, pMsg);
          break;
        case 0x1Au:
          result = InitClassCostRequest(n, pMsg);
          break;
        case 0x1Cu:
          result = CanSelectClassRequest(n, pMsg);
          break;
        default:
          result = subType == 33 && SelectPcBangRewardRequest(n, pMsg);
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
          result = BuyStoreRequest(n, pMsg);
          break;
        case 3:
          result = SellStoreRequest(n, pMsg);
          break;
        case 7:
          result = StoreListRequest(n, pMsg);
          break;
        case 9:
          result = ExchangeDalantForGoldRequest(n, pMsg);
          break;
        case 10:
          result = ExchangeGoldForDalantRequest(n, pMsg);
          break;
        case 16:
          result = LimitItemNumRequest(n, pMsg);
          break;
        case 18:
          result = TalikRecorverList(n, pMsg);
          break;
        case 20:
          result = PvpCashRecorverWithTalik(n, pMsg);
          break;
        case 22:
          result = PcBangPrimiumCouponRequest(n, pMsg);
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
          result = PartyReqBlockReport(n, pMsg);
          break;
        case 29:
          result = WhisperBlockReport(n, pMsg);
          break;
        case 30:
          result = TradeBlockReport(n, pMsg);
          break;
        case 33:
          result = GuildBattleBlockReport(n, pMsg);
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
          result = AutoMineMachineMng::OpenUI(autoMine, n);
          break;
        }
        case 16:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = AutoMineMachineMng::CloseUI(autoMine, n);
          break;
        }
        case 17:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = AutoMineMachineMng::StartWorkMachine(autoMine, n);
          break;
        }
        case 18:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = AutoMineMachineMng::StopWorkMachine(autoMine, n);
          break;
        }
        case 19:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = AutoMineMachineMng::SelectOreType(autoMine, n, pMsg);
          break;
        }
        case 20:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = AutoMineMachineMng::GetOutOre(autoMine, n, pMsg);
          break;
        }
        case 22:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = AutoMineMachineMng::MoveOrePos(autoMine, n, pMsg);
          break;
        }
        case 23:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = AutoMineMachineMng::BatteryCharge(autoMine, n, pMsg);
          break;
        }
        case 25:
        {
          AutoMineMachineMng *autoMine = AutoMineMachineMng::Instance();
          result = AutoMineMachineMng::OreMerge(autoMine, n, pMsg);
          break;
        }
        case 41:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = AutominePersonalMgr::make_storagebox(autoMine, n, pMsg);
          break;
        }
        case 43:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = AutominePersonalMgr::install(autoMine, n, pMsg);
          break;
        }
        case 45:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = AutominePersonalMgr::uninstall(autoMine, n, pMsg);
          break;
        }
        case 47:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = AutominePersonalMgr::selectore(autoMine, n, pMsg);
          break;
        }
        case 49:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = AutominePersonalMgr::insert_battery(autoMine, n, pMsg);
          break;
        }
        case 51:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = AutominePersonalMgr::extract_battery(autoMine, n, pMsg);
          break;
        }
        case 55:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = AutominePersonalMgr::pop_ore(autoMine, n, pMsg);
          break;
        }
        case 60:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = AutominePersonalMgr::Open_InvenUI(autoMine, n, pMsg);
          break;
        }
        case 62:
        {
          AutominePersonalMgr *autoMine = AutominePersonalMgr::instance();
          result = AutominePersonalMgr::Open_InfoUI(autoMine, n, pMsg);
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
          result = PartyJoinInvitationAnswer(n, pMsg);
          break;
        case 3:
          result = PartyJoinApplication(n, pMsg);
          break;
        case 5:
          result = PartyJoinApplicatiohAnswer(n, pMsg);
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
          result = PartyLockRequest(n, pMsg);
          break;
        case 27:
          result = AlterPartyLootShareRequest(n, pMsg);
          break;
        case 30:
          result = AwayPartyInvitation(n, pMsg);
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
          result = DTradeAnswerRequest(n, pMsg);
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
          result = GuildEstablishRequest(n, pMsg);
          break;
        case 4:
          result = GuildDownloadRequest(n, pMsg);
          break;
        case 5:
          result = GuildJoinApplyRequest(n, pMsg);
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
          result = TrunkHintAnswerRequest(n, pMsg);
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
          result = PatriarchElectProcessor::Doit(processor, _eRequestCandidateList, pOne, pMsg);
          break;
        }
        case 2:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = PatriarchElectProcessor::Doit(processor, _eRegCandidate, pOne, pMsg);
          break;
        }
        case 3:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = PatriarchElectProcessor::Doit(processor, _eVote, pOne, pMsg);
          break;
        }
        case 7:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = PatriarchElectProcessor::Doit(processor, _eQryAppoint, pOne, pMsg);
          break;
        }
        case 9:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = PatriarchElectProcessor::Doit(processor, _eReqAppoint, pOne, pMsg);
          break;
        }
        case 11:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = PatriarchElectProcessor::Doit(processor, _eRespAppoint, pOne, pMsg);
          break;
        }
        case 14:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = PatriarchElectProcessor::Doit(processor, _eReqDischarge, pOne, pMsg);
          break;
        }
        case 15:
        {
          CPlayer *pOne = &g_Player[n];
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          result = PatriarchElectProcessor::Doit(processor, _eReqPatriarchInform, pOne, pMsg);
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
      if (subType == 1)
      {
        CashItemRemoteStore *store = CashItemRemoteStore::Instance();
        result = CashItemRemoteStore::GoodsList(store, n, pMsg);
      }
      else if (subType == 3)
      {
        CashItemRemoteStore *store = CashItemRemoteStore::Instance();
        result = CashItemRemoteStore::Buy(store, n, pMsg);
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
        result = CNuclearBombMgr::Request_EnableNuclearControl(mgr, n, pMsg);
      }
      else if (subType == 2)
      {
        CNuclearBombMgr *mgr = CNuclearBombMgr::Instance();
        result = CNuclearBombMgr::Request_SelectDropPosition(mgr, n, pMsg);
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
      result = CNationSettingManager::RecvGameGuardData(manager, n, pMsgHeader, pMsg);
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

bool CNetworkEX::NewPosStartRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }

  const unsigned __int8 mapInMode = static_cast<unsigned __int8>(*pBuf);
  if (mapInMode < 9u)
  {
    player->pc_NewPosStart();
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: NewPosStartRequest()..  if(pRecv->byMapInMode >= mapin_type_num)",
    charName);
  return false;
}

bool CNetworkEX::BaseDownloadRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bBaseDownload)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: BaseDownloadRequest()..  if(pOne->m_bBaseDownload)",
      charName);
    return false;
  }

  player->SendMsg_BaseDownloadResult();
  return true;
}

bool CNetworkEX::InvenDownloadRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bInvenDownload)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: InvenDownloadRequest()..  if(pOne->m_bInvenDownload)",
      charName);
    return false;
  }

  player->SendMsg_InvenDownloadResult();
  return true;
}

bool CNetworkEX::CumDownloadRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bCumDownload)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: CumDownloadRequest()..  if(pOne->m_bCumDownload)",
      charName);
    return false;
  }

  player->SendMsg_CumDownloadResult();
  return true;
}

bool CNetworkEX::ForceDownloadRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bForceDownload)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ForceDownloadRequest()..  if(pOne->m_bForceDownload)",
      charName);
    return false;
  }

  player->SendMsg_ForceDownloadResult();
  return true;
}

bool CNetworkEX::QuestDownloadRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bQuestDownload)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: QuestDownloadRequest()..  if(pOne->m_bQuestDownload)",
      charName);
    return false;
  }

  player->SendMsg_QuestDownloadResult();
  player->SendMsg_QuestHistoryDownloadResult();
  return true;
}

bool CNetworkEX::SpecialDownloadRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bSpecialDownload)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
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

    const char *charName = CPlayerDB::GetCharNameA(&dst->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: OtherShapeRequest()..  if(pRecv->wIndex >= MAX_PLAYER)",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&dst->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: OtherShapeRequest()..  if(pRecv->byReqType != _other_shape_request_clzo::SHAPE_ALL && pRecv->byReqType !"
    "= _other_shape_request_clzo::SHAPE_PART)",
    charName);
  return false;
}

bool CNetworkEX::Revival(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_Revival(true);
  }
  return true;
}

bool CNetworkEX::ExitWorldRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  if (m_bUseFG)
  {
    _socket *socket = GetSocket(0, n);
    if (socket)
    {
      _CcrFG_rs_CloseUserContext(&socket->m_hFGContext);
    }
  }

  player->pc_ExitWorldRequest();
  return true;
}

bool CNetworkEX::LinkBoardDownloadRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (player->m_bLoad)
  {
    player->pc_LinkBoardRequest();
  }
  return true;
}

bool CNetworkEX::MacroDownLoadRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (player->m_bLoad)
  {
    player->SendMsg_MacroRequest();
  }
  return true;
}

bool CNetworkEX::AMP_DownloadRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bLoad)
  {
    return true;
  }
  if (player->m_bAMPInvenDownload)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: AMP_InvenDownloadRequest()..  if(pOne->m_bAMPInvenDownload)",
      charName);
    return false;
  }

  player->SendMsg_AMPInvenDownloadResult();
  return true;
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
    CLogFile::Write(
      &m_LogFile,
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
        CLogFile::Write(
          &m_LogFile,
          "KeyCheckError.. Socket( %d ): EnterWorldRequest().. if( strncmp( CMainThread::ms_szClientVerCheck, pRecv->szClientVerCheckKey, CMainThread::eClinetCheckMax ) )",
          n);
        resultCode = static_cast<unsigned __int8>(-14);
      }
    }
    else
    {
      CLogFile::Write(
        &m_LogFile,
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
    CNetProcess::LoadSendMsg(
      g_Network.m_pProcess[0],
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
    CNetProcess::StartSpeedHackCheck(m_pProcess[0], n, g_UserDB[n].m_szAccountID);
  }
  else
  {
    Close(0, n, false, nullptr);
  }
  return true;
}

bool CNetworkEX::MoveLobbyRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
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
  (void)pBuf;
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
  if (!g_UserDB[n].m_bActive)
  {
    return true;
  }

  char charName[17]{};
  memcpy_0(charName, pBuf + 1, 0x10u);
  charName[16] = '\0';

  if (strlen_0(charName) <= 0x10)
  {
    if (static_cast<unsigned __int8>(pBuf[0]) < 3u)
    {
      if (static_cast<unsigned __int8>(pBuf[18]) < 5u)
      {
        if (strlen_0(pBuf + 19) <= 4)
        {
          if (Major_Add_Character)
          {
            return false;
          }

          if (!g_UserDB[n].Insert_Char_Request(
                charName,
                static_cast<unsigned __int8>(pBuf[0]),
                static_cast<unsigned __int8>(pBuf[18]),
                pBuf + 19,
                *reinterpret_cast<unsigned int *>(pBuf + 24)))
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

  CLogFile::Write(
    &m_LogFile,
    "odd.. id( %s ): AddCharRequest().. if(strlen(pRecv->szCharName) > max_name_len)",
    g_UserDB[n].m_szAccountID);
  return false;
}

bool CNetworkEX::DelCharRequest(unsigned int n, char *pBuf)
{
  unsigned __int8 *recv = reinterpret_cast<unsigned __int8 *>(pBuf);
  if (!g_UserDB[n].m_bActive)
  {
    return true;
  }
  if (*recv < 3u)
  {
    if (!g_UserDB[n].Delete_Char_Request(*recv))
    {
      char buffer[144]{};
      sprintf(buffer, "CLOSE>> DelCharRequest ID:%s", g_UserDB[n].m_szAccountID);
      Close(0, n, false, buffer);
    }
    return true;
  }

  CLogFile::Write(
    &m_LogFile,
    "odd.. id( %s ): DelCharRequest().. if(pRecv->bySlotIndex >= MAX_CHAR_PER_WORLDUSER)",
    g_UserDB[n].m_szAccountID);
  return false;
}

bool CNetworkEX::SelCharRequest(unsigned int n, char *pBuf)
{
  unsigned __int8 *recv = reinterpret_cast<unsigned __int8 *>(pBuf);
  if (!g_UserDB[n].m_bActive)
  {
    return true;
  }
  if (g_UserDB[n].m_byUserDgr || g_UserDB[n].m_byUILock == 2)
  {
    if (*recv < 3u)
    {
      if (!g_UserDB[n].Select_Char_Request(*recv))
      {
        char buffer[144]{};
        sprintf(buffer, "CLOSE>> SelCharRequest ID:%s", g_UserDB[n].m_szAccountID);
        Close(0, n, false, buffer);
      }
      return true;
    }

    CLogFile::Write(
      &m_LogFile,
      "odd.. id( %s ): SelCharRequest().. if(pRecv->bySlotIndex >= MAX_CHAR_PER_WORLDUSER)",
      g_UserDB[n].m_szAccountID);
    return false;
  }

  CLogFile::Write(
    &m_LogFile,
    "odd.. id( %s ): SelCharRequest().. UI Lock Not Certified!",
    g_UserDB[n].m_szAccountID);
  return false;
}

bool CNetworkEX::TutorialProcessReport(int n, char *pBuf)
{
  (void)pBuf;
  if (g_UserDB[n].m_bActive)
  {
    CUserDB::s_MgrLobbyHistory.tutorial_process_report_recv(g_UserDB[n].m_szLobbyHistoryFileName);
    g_UserDB[n].m_dwOperLobbyTime = GetLoopTime();
  }
  return true;
}

bool CNetworkEX::AliveCharRequest(int n, char *pBuf)
{
  if (!g_UserDB[n].m_bActive)
  {
    return true;
  }

  char charName[17]{};
  memcpy_0(charName, pBuf + 5, 0x10u);
  charName[16] = '\0';

  if (strlen_0(charName) <= 0x10)
  {
    if (static_cast<unsigned __int8>(pBuf[22]) < 3u)
    {
      if (!pBuf[0] || pBuf[0] == 1)
      {
        g_UserDB[n].Alive_Char_Request(
          static_cast<unsigned __int8>(pBuf[0]),
          *reinterpret_cast<unsigned int *>(pBuf + 1),
          charName,
          static_cast<unsigned __int8>(pBuf[22]));
        return true;
      }

      CLogFile::Write(&m_LogFile, "odd.. AliveCharRequest() .. if(pRecv->byCase != 0 && pRecv->byCase != 1)");
      return false;
    }

    CLogFile::Write(&m_LogFile, "odd.. AliveCharRequest() .. if(pRecv->bySlotIndex >= MAX_CHAR_PER_WORLDUSER)");
    return false;
  }

  CLogFile::Write(
    &m_LogFile,
    "odd.. id( %s ): AddCharRequest().. if(strlen(pRecv->szCharName) > max_name_len)",
    g_UserDB[n].m_szAccountID);
  return false;
}

bool CNetworkEX::NotifyLocalTimeRequest(int n, char *pBuf)
{
  (void)pBuf;
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
    CNetProcess::LoadSendMsg(
      g_Network.m_pProcess[0],
      player->m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&result),
      len);
    return true;
  }

  result.byRet = 1;
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    player->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&result),
    len);
  return true;
}

bool CNetworkEX::ChatOperatorRequest(unsigned int n, char *pBuf)
{
  unsigned __int8 *recv = reinterpret_cast<unsigned __int8 *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  if (*recv < 3u || *recv == 0xFF)
  {
    if (recv[1] == 0xFF)
    {
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: ChatOperatorRequest()..  if(pRecv->bySize > max_message_size)",
        charName);
    }
    else
    {
      char chatData[272]{};
      memcpy_0(chatData, recv + 2, recv[1]);
      chatData[recv[1]] = 0;
      player->pc_ChatOperatorRequest(*recv, chatData);
    }
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ChatOperatorRequest()..  if(pRecv->byRaceCode >= RACE_NUM && pRecv->byRaceCode != 0xFF)",
    charName);
  return false;
}

bool CNetworkEX::ChatCircleRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }
  if (_effect_parameter::GetEff_Have(&player->m_EP, 50) > 0.0)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ChatCircleRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, recv + 1, size);
    chatData[size] = 0;
    player->pc_ChatCircleRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatFarRequest(unsigned int n, char *pBuf)
{
  void *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }
  if (_effect_parameter::GetEff_Have(&player->m_EP, 50) > 0.0)
  {
    return true;
  }

  if (reinterpret_cast<unsigned __int8 *>(recv)[17] == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(&m_LogFile, "odd.. %s: ChatFarRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char name[56]{};
    char chatData[272]{};
    memcpy_0(name, recv, 0x10u);
    name[16] = 0;
    memcpy_0(chatData, static_cast<char *>(recv) + 18, reinterpret_cast<unsigned __int8 *>(recv)[17]);
    chatData[reinterpret_cast<unsigned __int8 *>(recv)[17]] = 0;
    player->pc_ChatFarRequest(name, chatData);
  }
  return true;
}

bool CNetworkEX::ChatPartyRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }
  if (_effect_parameter::GetEff_Have(&player->m_EP, 50) > 0.0)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(&m_LogFile, "odd.. %s: ChatPartyRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, recv + 1, size);
    chatData[size] = 0;
    player->pc_ChatPartyRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatRaceRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }
  if (_effect_parameter::GetEff_Have(&player->m_EP, 50) > 0.0)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(&m_LogFile, "odd.. %s: ChatRaceRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, recv + 1, size);
    chatData[size] = 0;
    player->pc_ChatRaceRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatCheatRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(&m_LogFile, "odd.. %s: ChatCheatRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char command[272]{};
    memcpy_0(command, recv + 1, size);
    command[size] = 0;
    ProcessCheatCommand(player, command);
  }
  return true;
}

bool CNetworkEX::ChatManageRequest(unsigned int n, char *pBuf)
{
  (void)n;
  (void)pBuf;
  // this is not a stub
  return true;
}

bool CNetworkEX::ChatMgrWhisperRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ChatMgrWhisperRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, recv + 1, size);
    chatData[size] = 0;
    player->pc_ChatMgrWhisperRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatMapRecvYesOrNo(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (player->m_bLoad)
  {
    player->m_bRecvMapChat = *recv == 0;
  }
  return true;
}

bool CNetworkEX::ChatMapRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(&m_LogFile, "odd.. %s: ChatMapRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, recv + 1, size);
    chatData[size] = 0;
    player->pc_ChatMapRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatRaceBossRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ChatRaceBossRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, recv + 1, size);
    chatData[size] = 0;
    player->pc_ChatRaceBossRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatGuildEstSenRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ChatGuildEstSenRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, recv + 1, size);
    chatData[size] = 0;
    player->pc_ChatGuildEstSenRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatRePresentationRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ChatRePresentationRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, recv + 1, size);
    chatData[size] = 0;
    player->pc_ChatRePresentationRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatAllRecvYesOrNo(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (player->m_bLoad)
  {
    player->m_bRecvAllChat = *recv == 0;
  }
  return true;
}

bool CNetworkEX::ChatAllRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(&m_LogFile, "odd.. %s: ChatAllRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, recv + 1, size);
    chatData[size] = 0;
    player->pc_ChatAllRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatGreetingMsg_GM(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }
  if (player->m_pUserDB->m_byUserDgr < 2u)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ChatGreetingMsg_GM()..  if(pRecv->bySize > max_message_size)",
      charName);
    return true;
  }

  if (IsSQLValidString(recv + 1))
  {
    char msg[272]{};
    memcpy_0(msg, recv + 1, size);
    msg[size] = 0;
    char *charName = CPlayerDB::GetCharNameW(&player->m_Param);
    CMainThread::pc_SetMainGreetingMsg(&g_Main, charName, msg);
  }
  else
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &g_Main.m_logSystemError,
      "CNetworkEX::ChatGreetingMsg_GM() : %u(%s) ::IsSQLValidString( pRecv->wszChatData ) Invalid!",
      player->m_dwObjSerial,
      charName);
  }
  return true;
}

bool CNetworkEX::ChatGreetingMsg_RACE(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ChatGreetingMsg_RACE()..  if(pRecv->bySize > max_message_size)",
      charName);
    return true;
  }

  const unsigned int charSerial = CPlayerDB::GetCharSerial(&player->m_Param);
  const int raceCode = CPlayerDB::GetRaceCode(&player->m_Param);
  CPvpUserAndGuildRankingSystem *rank = CPvpUserAndGuildRankingSystem::Instance();
  const unsigned int bossSerial = CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(rank, raceCode, 0);
  if (charSerial != bossSerial)
  {
    return true;
  }

  if (IsSQLValidString(recv + 1))
  {
    char msg[272]{};
    memcpy_0(msg, recv + 1, size);
    msg[size] = 0;
    char *bossName = CPlayerDB::GetCharNameA(&player->m_Param);
    const int senderRace = CPlayerDB::GetRaceCode(&player->m_Param);
    CMainThread::pc_SetRaceGreetingMsg(&g_Main, senderRace, bossName, msg);
  }
  else
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &g_Main.m_logSystemError,
      "CNetworkEX::ChatGreetingMsg_RACE() : %u(%s) ::IsSQLValidString( pRecv->wszChatData ) Invalid!",
      player->m_dwObjSerial,
      charName);
  }
  return true;
}

bool CNetworkEX::ChatGreetingMsg_GUILD(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ChatGreetingMsg_GUILD()..  if(pRecv->bySize > max_message_size)",
      charName);
    return true;
  }

  if (player->m_Param.m_pGuild)
  {
    const unsigned int masterSerial = CGuild::GetGuildMasterSerial(player->m_Param.m_pGuild);
    if (masterSerial != static_cast<unsigned int>(-1))
    {
      const unsigned int charSerial = CPlayerDB::GetCharSerial(&player->m_Param);
      if (masterSerial == charSerial)
      {
        if (IsSQLValidString(recv + 1))
        {
          char msg[272]{};
          memcpy_0(msg, recv + 1, size);
          msg[size] = 0;
          if (player->m_Param.m_pGuild)
          {
            CGuild::SetGreetingmsg_GUILD(player->m_Param.m_pGuild, msg);
          }
          return true;
        }

        const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
        CLogFile::Write(
          &g_Main.m_logSystemError,
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
  unsigned __int8 *recv = reinterpret_cast<unsigned __int8 *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  if (recv[1] == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(&m_LogFile, "odd.. %s: ChatTradeMsg()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char tradeMsg[272]{};
    memcpy_0(tradeMsg, recv + 2, recv[1]);
    tradeMsg[recv[1]] = 0;
    player->pc_ChatTradeRequestMsg(*recv, tradeMsg);
  }
  return true;
}

bool CNetworkEX::ChatGuildRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }
  if (_effect_parameter::GetEff_Have(&player->m_EP, 50) > 0.0)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(recv[4]);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(&m_LogFile, "odd.. %s: ChatGuildRequest()..  if(pRecv->bySize > max_message_size)", charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, recv + 5, size);
    chatData[size] = 0;
    player->pc_ChatGuildRequest(*reinterpret_cast<unsigned int *>(recv), chatData);
  }
  return true;
}

bool CNetworkEX::ChatMultiFarRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  unsigned __int8 transNum = 0;
  const unsigned __int8 *src = reinterpret_cast<unsigned __int8 *>(recv + 2);
  memcpy_0(&transNum, src, 1u);
  ++src;

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
        const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
        CLogFile::Write(&m_LogFile, "odd.. %s: ChatMultiFarRequest()..  if(byNameLen > max_name_len)", charName);
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
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(&m_LogFile, "odd.. %s: ChatMultiFarRequest()..  if(byMsgSize >= max_message_size)", charName);
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

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(&m_LogFile, "odd.. %s: ChatMultiFarRequest()..  if(byTransNum > max_multi_far_msg)", charName);
  return true;
}

bool CNetworkEX::ChatRaceBossCryRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ChatRaceBossCryRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, recv + 1, size);
    chatData[size] = 0;
    player->pc_ChatRaceBossCryRequest(chatData);
  }
  return true;
}

bool CNetworkEX::ChatGmNoticeRequest(unsigned int n, char *pBuf)
{
  char *recv = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int8 size = static_cast<unsigned __int8>(*recv);
  if (size == 0xFF)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ChatGmNoticeRequest()..  if(pRecv->bySize > max_message_size)",
      charName);
  }
  else
  {
    char chatData[272]{};
    memcpy_0(chatData, recv + 1, size);
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
