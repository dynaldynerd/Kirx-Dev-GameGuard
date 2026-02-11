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
#include "CPcBangFavor.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CTotalGuildRankManager.h"
#include "CWeeklyGuildRankManager.h"
#include "CGuild.h"
#include "CheatCommands.h"
#include "PatriarchElectProcessor.h"
#include "CPlayer.h"
#include "CRecallEffectController.h"
#include "CItemStoreManager.h"
#include "CMapData.h"
#include "CGuildRoomSystem.h"
#include "CReturnGateController.h"
#include "CUserDB.h"
#include "CRaceBossMsgController.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "CHRID.h"
#include "cancel_raceboss_msg_result_zoct.h"
#include "character_disconnect_result_wrac.h"
#include "connection_status_result_zoct.h"
#include "combine_ex_item_accept_request_clzo.h"
#include "combine_ex_item_request_clzo.h"
#include "combine_item_request_clzo.h"
#include "force_inven_change_request_clzo.h"
#include "animus_inven_change_request_clzo.h"
#include "animus_recall_request_clzo.h"
#include "animus_command_request_clzo.h"
#include "animus_target_request_clzo.h"
#include "res_separation_request_clzo.h"
#include "res_division_request_clzo.h"
#include "potionsocket_separation_request_clzo.h"
#include "potionsocket_division_request_clzo.h"
#include "alter_item_slot_request_clzo.h"
#include "alter_link_slot_request_clzo.h"
#include "pvp_rank_list_request_clzo.h"
#include "mode_change_request_clzo.h"
#include "gesture_request_clzo.h"
#include "alter_window_info_request_clzo.h"
#include "unit_frame_buy_request_clzo.h"
#include "unit_sell_request_clzo.h"
#include "unit_part_tuning_request_clzo.h"
#include "unit_frame_repair_request_clzo.h"
#include "unit_bullet_fill_request_clzo.h"
#include "unit_pack_fill_request_clzo.h"
#include "unit_delivery_request_clzo.h"
#include "unit_return_request_clzo.h"
#include "unit_take_request_clzo.h"
#include "unit_leave_request_clzo.h"
#include "unit_bullet_replace_request_clzo.h"
#include "set_target_object_request_clzo.h"
#include "object_server_pos_request_clzo.h"
#include "release_target_object_request_clzo.h"
#include "player_macro_update_request_clzo.h"
#include "total_guild_rank_request_clzo.h"
#include "weekly_guild_rank_request_clzo.h"
#include "set_raceboss_cry_msg_request_clzo.h"
#include "notify_raceboss_cry_msg_request_zocl.h"
#include "set_group_target_object_request_clzo.h"
#include "release_group_target_object_request_clzo.h"
#include "set_group_map_point_request_clzo.h"
#include "select_waited_quest_report_clzo.h"
#include "quest_select_reward_repart_clzo.h"
#include "brief_pass_report_clzo.h"
#include "npc_quest_request_clzo.h"
#include "npc_quest_list_request_clzo.h"
#include "quest_give_up_request_clzo.h"
#include "pt_request_punishment_clzo.h"
#include "uilock_init_request_clzo.h"
#include "uilock_user_certify_request_clzo.h"
#include "uilock_update_info_request_clzo.h"
#include "uilock_find_pw_request_clzo.h"
#include "mine_start_request_clzo.h"
#include "mine_cancle_request_clzo.h"
#include "ore_cutting_request_clzo.h"
#include "ore_into_bag_request_clzo.h"
#include "cutting_complete_request_clzo.h"
#include "qry_case_select_timelimit_info.h"
#include "buy_store_request_clzo.h"
#include "sell_store_request_clzo.h"
#include "force_request_clzo.h"
#include "skill_request_clzo.h"
#include "class_skill_request_clzo.h"
#include "make_tower_request_clzo.h"
#include "back_tower_request_clzo.h"
#include "make_trap_request_clzo.h"
#include "decide_recall_request_clzo.h"
#include "back_trap_request_clzo.h"
#include "force_recall_teleport_request_clzo.h"
#include "skill_recall_teleport_request_clzo.h"
#include "class_skill_recall_teleport_request_clzo.h"
#include "thorw_skill_request_clzo.h"
#include "d_trade_add_request_clzo.h"
#include "d_trade_answer_request_clzo.h"
#include "d_trade_ask_request_clzo.h"
#include "d_trade_bet_request_clzo.h"
#include "d_trade_cancle_request_clzo.h"
#include "d_trade_del_request_clzo.h"
#include "d_trade_lock_request_clzo.h"
#include "d_trade_ok_request_clzo.h"
#include "enter_world_request_zone.h"
#include "enter_world_result_zone.h"
#include "make_item_request_clzo.h"
#include "notify_local_time_result_zocl.h"
#include "other_shape_request_clzo.h"
#include "server_notify_inform_zone.h"
#include "talik_crystal_exchange_clzo.h"
#include "w_name.h"

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
          result = this->NewPosStartRequest(n, pMsg);
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
          result = this->GotoAvatorRequest(n, pMsg);
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
          result = this->ThrowStorageRequest(n, pMsg);
          break;
        case 5:
          result = this->UsePotionRequest(n, pMsg);
          break;
        case 7:
          result = this->EquipPartRequest(n, pMsg);
          break;
        case 9:
          result = this->EmbellishRequest(n, pMsg);
          break;
        case 11:
          result = this->OffPartRequest(n, pMsg);
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
          result = this->UseRecallTeleportItemRequest(n, pMsg);
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
          result = this->InitClassRequest(n, pMsg);
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

bool CNetworkEX::NextPoint(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  char *packet = pBuf;
  if (!player->m_bOper)
  {
    return true;
  }

  DeCrypt_Move(packet, 22, static_cast<unsigned __int8>(player->m_byPlusKey + 1), player->m_wXorKey + 1);
  const unsigned int recvMoveCount = *reinterpret_cast<unsigned int *>(packet + 13);
  if (player->m_dwMoveCount < recvMoveCount)
  {
    if (recvMoveCount - player->m_dwMoveCount > 100u)
    {
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &g_Main.m_logMove,
        "move count hack(start) warning! >> %s (one: %d, recv: %d)",
        charName,
        player->m_dwMoveCount,
        recvMoveCount);
    }
    player->m_dwMoveCount = recvMoveCount;

    const unsigned __int8 moveType = static_cast<unsigned __int8>(*packet);
    if (moveType == 0 || moveType == 1 || moveType == 2)
    {
      float targetPos[3]{};
      targetPos[0] = static_cast<float>(*reinterpret_cast<short *>(packet + 17));
      targetPos[1] = *reinterpret_cast<float *>(packet + 5);
      targetPos[2] = static_cast<float>(*reinterpret_cast<short *>(packet + 19));
      player->pc_MoveNext(moveType, reinterpret_cast<float *>(packet + 1), targetPos, packet[21]);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: NextPoint()..  if(pRecv->byMoveType != move_type_walk && pRecv->byMoveType != move_type_run && pRecv->byMoveType != move_type_fly)",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &g_Main.m_logMove,
    "move count hack(start) >> %s (one: %d, recv: %d)",
    charName,
    player->m_dwMoveCount,
    recvMoveCount);
  return false;
}

bool CNetworkEX::RealMovPosRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  char *packet = pBuf;
  if (!player->m_bOper)
  {
    return true;
  }

  DeCrypt_Move(packet, 16, static_cast<unsigned __int8>(player->m_byPlusKey + 1), player->m_wXorKey + 2);
  const unsigned int recvMoveCount = *reinterpret_cast<unsigned int *>(packet);
  if (player->m_dwMoveCount < recvMoveCount)
  {
    if (recvMoveCount - player->m_dwMoveCount > 100u)
    {
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &g_Main.m_logMove,
        "move count hack(real) warning! >> %s (one: %d, recv: %d)",
        charName,
        player->m_dwMoveCount,
        recvMoveCount);
    }
    player->m_dwMoveCount = recvMoveCount;
    player->pc_RealMovPos(reinterpret_cast<float *>(packet + 4));
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &g_Main.m_logMove,
    "move count hack(real) >> %s (one: %d, recv: %d)",
    charName,
    player->m_dwMoveCount,
    recvMoveCount);
  return false;
}

bool CNetworkEX::Stop(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  char *packet = pBuf;
  if (!player->m_bOper)
  {
    return true;
  }

  DeCrypt_Move(packet, 16, static_cast<unsigned __int8>(player->m_byPlusKey + 1), player->m_wXorKey + 19);
  const unsigned int recvMoveCount = *reinterpret_cast<unsigned int *>(packet);
  if (player->m_dwMoveCount < recvMoveCount)
  {
    if (recvMoveCount - player->m_dwMoveCount > 100u)
    {
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &g_Main.m_logMove,
        "move count hack(stop) warning! >> %s (one: %d, recv: %d)",
        charName,
        player->m_dwMoveCount,
        recvMoveCount);
    }
    player->m_dwMoveCount = recvMoveCount;
    player->pc_MoveStop(reinterpret_cast<float *>(packet + 4));
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &g_Main.m_logMove,
    "move count hack(stop) >> %s (one: %d, recv: %d)",
    charName,
    player->m_dwMoveCount,
    recvMoveCount);
  return false;
}

bool CNetworkEX::GotoBasePortalRequest(unsigned int n, char *pBuf)
{
  auto *packet = reinterpret_cast<unsigned __int16 *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  player->pc_GotoBasePortalRequest(*packet);
  return true;
}

bool CNetworkEX::GotoAvatorRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  char nameBuffer[17]{};
  memcpy_0(nameBuffer, pBuf, 16);
  nameBuffer[16] = '\0';
  if (strlen_0(nameBuffer) <= 16)
  {
    if (player->m_byUserDgr)
    {
      player->pc_GotoAvatorRequest(nameBuffer);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: GotoAvatorRequest()..  if(pOne->m_byUserDgr == USER_DEGREE_STD)",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: GotoAvatorRequest()..  if(strlen(pRecv->szAvatorName) > max_name_len)",
    charName);
  return false;
}

bool CNetworkEX::MovePortalRequest(unsigned int n, char *pBuf)
{
  char *packet = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  player->pc_MovePortal(static_cast<int>(static_cast<unsigned __int8>(*packet)), reinterpret_cast<unsigned __int16 *>(packet + 1));
  return true;
}

bool CNetworkEX::RegistBindRequest(unsigned int n, char *pBuf)
{
  char *packet = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int16 npcIndex = *reinterpret_cast<unsigned __int16 *>(packet);
  CItemStoreManager *storeManager = CItemStoreManager::Instance();
  const int recordNum = static_cast<int>(storeManager->m_tblItemStore.GetRecordNum());
  if (npcIndex < recordNum)
  {
    CItemStore *store = nullptr;
    const int mapSerial = CMapData::GetMapCode(player->m_pCurMap);
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

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: RegistBindRequest()..  if(pRecv->wNPCIndex >= CItemStoreManager::Instance()->m_tblItemStore.GetRecordNum())",
    charName);
  return false;
}

bool CNetworkEX::EnterReturnGateRequest(unsigned int n, char *pBuf)
{
  char *packet = pBuf;
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    const unsigned int gateIndex = *reinterpret_cast<unsigned __int16 *>(packet);
    CReturnGateController::Instance()->Enter(gateIndex, player);
  }
  return true;
}

bool CNetworkEX::MoveTypeChangeRequeset(unsigned int n, char *pBuf)
{
  auto *packet = reinterpret_cast<unsigned __int8 *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (*packet == 0 || *packet == 1 || *packet == 2)
  {
    player->pc_MoveModeChangeRequest(*packet);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: MoveTypeChangeRequeset()..  if(pRecv->byMoveType != move_type_walk && pRecv->byMoveType != move_type_run)",
    charName);
  return false;
}

bool CNetworkEX::MoveInfoRequeset(unsigned int n, char *pBuf)
{
  (void)n;
  (void)pBuf;
  // this is not a stub
  return true;
}

bool CNetworkEX::PlayerInfoResult(unsigned int n, char *pBuf)
{
  (void)n;
  (void)pBuf;
  // this is not a stub
  return true;
}

bool CNetworkEX::SelectClassRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int16 selClassIndex = *reinterpret_cast<unsigned __int16 *>(pBuf);
  const int recordNum = CRecordData::GetRecordNum(&g_Main.m_tblClass);
  if (selClassIndex < recordNum)
  {
    const unsigned __int8 selectRewardItem = static_cast<unsigned __int8>(pBuf[2]);
    if (selectRewardItem == 0xFF || selectRewardItem < 9u)
    {
      player->pc_SelectClassRequest(selClassIndex, selectRewardItem);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: SelectClassRequest()..  if(pRecv->bySelectRewardItem >= max_class_bns_item)",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: SelectClassRequest()..  if(pRecv->wSelClassIndex >= g_Main.m_tblClass.GetRecordNum())",
    charName);
  return false;
}

bool CNetworkEX::InitClassRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  unsigned __int8 resultCode = 0;
  if (*pBuf)
  {
    if (static_cast<unsigned __int8>(*pBuf) == 2)
    {
      resultCode = g_Main.m_pRFEvent_ClassRefine->DoEvent(g_Main.m_pRFEvent_ClassRefine, player);
    }
  }
  else
  {
    resultCode = player->pc_InitClassRequest();
  }

  unsigned __int8 type[2]{11, 25};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], player->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&resultCode), 1u);
  return true;
}

bool CNetworkEX::InitClassCostRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    unsigned int cost = player->GetInitClassCost();
    unsigned __int8 type[2]{11, 27};
    CNetProcess::LoadSendMsg(
      g_Network.m_pProcess[0],
      player->m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&cost),
      4u);
  }
  return true;
}

bool CNetworkEX::CanSelectClassRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    unsigned __int8 isRealClassUp = 0;
    const unsigned __int8 resultCode = player->pc_CanSelectClassRequest(reinterpret_cast<bool *>(&isRealClassUp));
    unsigned __int8 msg[2]{resultCode, isRealClassUp};
    unsigned __int8 type[2]{11, 29};
    CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], player->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(msg), 2u);
  }
  return true;
}

bool CNetworkEX::SelectPcBangRewardRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    const unsigned int rewardIndex = *reinterpret_cast<unsigned int *>(pBuf);
    unsigned __int8 *selectItemIndex = reinterpret_cast<unsigned __int8 *>(pBuf + 4);
    CPcBangFavor *pcBang = CPcBangFavor::Instance();
    CPcBangFavor::PcBangGiveItem(pcBang, player, rewardIndex, selectItemIndex, 5);
  }
  return true;
}

bool CNetworkEX::BuyStoreRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  CItemStoreManager *storeManager = CItemStoreManager::Instance();
  const unsigned int recordNum = CRecordData::GetRecordNum(&storeManager->m_tblItemStore);
  const unsigned int storeIndex = *reinterpret_cast<unsigned int *>(pBuf);
  if (storeIndex >= recordNum)
  {
    return true;
  }

  const unsigned __int8 buyNum = static_cast<unsigned __int8>(pBuf[4]);
  if (buyNum > 0x64u)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: BuyStoreRequest().. if(pRecv->byBuyNum > trade_item_max_num)",
      charName);
    return false;
  }
  if (!buyNum)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(&m_LogFile, "odd.. %s: BuyStoreRequest().. if(pRecv->byBuyNum == 0)", charName);
    return false;
  }

  CMapItemStoreList *storeList = nullptr;
  if (player->m_pCurMap->m_pMapSet->m_nMapType == 1)
  {
    if (player->m_Param.m_pGuild)
    {
      CGuildRoomSystem *guildRoom = CGuildRoomSystem::GetInstance();
      if (CGuildRoomSystem::IsGuildRoomMemberIn(
            guildRoom,
            player->m_Param.m_pGuild->m_dwSerial,
            player->m_ObjID.m_wIndex,
            player->m_pUserDB->m_dwSerial))
      {
        CItemStoreManager *manager = CItemStoreManager::Instance();
        storeList = CItemStoreManager::GetInstanceStoreListBySerial(manager, player->m_Param.m_pGuild->m_dwSerial);
      }
    }
  }
  else
  {
    unsigned __int8 mapCode = static_cast<unsigned __int8>(-1);
    if (*reinterpret_cast<unsigned int *>(pBuf + 5))
    {
      const int raceCode = CPlayerDB::GetRaceCode(&player->m_Param);
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
      mapCode = static_cast<unsigned __int8>(CMapData::GetMapCode(player->m_pCurMap));
    }

    CItemStoreManager *manager = CItemStoreManager::Instance();
    storeList = CItemStoreManager::GetMapItemStoreListBySerial(manager, mapCode);
  }

  if (!storeList)
  {
    return true;
  }

  CItemStore *store = CMapItemStoreList::GetItemStoreFromRecIndex(storeList, storeIndex);
  if (!store)
  {
    return true;
  }

  for (int j = 0; j < buyNum; ++j)
  {
    const unsigned int goodSerial = *reinterpret_cast<unsigned int *>(&pBuf[6 * j + 10]);
    if (goodSerial >= static_cast<unsigned int>(store->m_nStorageItemNum))
    {
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: BuyStoreRequest().. if(pRecv->OfferList[i].dwGoodSerial >= pStore->m_nStorageItemNum)",
        charName);
      return false;
    }
    const unsigned __int8 amount = static_cast<unsigned __int8>(pBuf[6 * j + 14]);
    if (!amount || amount > 0x63u)
    {
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: BuyStoreRequest().. if(pRecv->OfferList[i].byAmount < 1 || pRecv->OfferList[i].byAmount > 99)",
        charName);
      return false;
    }
    const unsigned __int8 storageCode = static_cast<unsigned __int8>(pBuf[6 * j + 9]);
    if (storageCode >= 8u)
    {
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: BuyStoreRequest().. if(pRecv->OfferList[i].byStorageCode >= _STORAGE_POS::STORAGE_NUM)",
        charName);
      return false;
    }
  }

  player->pc_BuyItemStore(
    store,
    buyNum,
    reinterpret_cast<_buy_store_request_clzo::_list *>(pBuf + 9),
    *reinterpret_cast<unsigned int *>(pBuf + 5));
  return true;
}

bool CNetworkEX::SellStoreRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  CItemStoreManager *storeManager = CItemStoreManager::Instance();
  const unsigned int recordNum = CRecordData::GetRecordNum(&storeManager->m_tblItemStore);
  const unsigned int storeIndex = *reinterpret_cast<unsigned int *>(pBuf);
  if (storeIndex >= recordNum)
  {
    return true;
  }

  const unsigned __int8 sellNum = static_cast<unsigned __int8>(pBuf[4]);
  if (sellNum > 0x64u)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: SellStoreRequest() : pRecv->bySellNum(%d) >= trade_item_max_num(%d)",
      charName,
      sellNum,
      100);
    return false;
  }
  if (!sellNum)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(&m_LogFile, "odd.. %s: SellStoreRequest() : if(pRecv->bySellNum == 0)", charName);
    return false;
  }

  for (int j = 0; j < sellNum; ++j)
  {
    const unsigned __int8 amount = static_cast<unsigned __int8>(pBuf[4 * j + 12]);
    if (!amount || amount > 0x63u)
    {
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: SellStoreRequest() : pRecv->Item[i].byAmount (%d)",
        charName,
        amount);
      return false;
    }
    const unsigned __int8 storageCode = static_cast<unsigned __int8>(pBuf[4 * j + 9]);
    if (storageCode >= 8u)
    {
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: SellStoreRequest() : pRecv->Item[i].byStorageCode (%d)",
        charName,
        storageCode);
      return false;
    }
  }

  unsigned __int8 mapCode = static_cast<unsigned __int8>(-1);
  if (*reinterpret_cast<unsigned int *>(pBuf + 5))
  {
    const int raceCode = CPlayerDB::GetRaceCode(&player->m_Param);
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
    mapCode = static_cast<unsigned __int8>(CMapData::GetMapCode(player->m_pCurMap));
  }

  CItemStoreManager *manager = CItemStoreManager::Instance();
  CMapItemStoreList *storeList = CItemStoreManager::GetMapItemStoreListBySerial(manager, mapCode);
  if (!storeList)
  {
    return true;
  }

  CItemStore *store = CMapItemStoreList::GetItemStoreFromRecIndex(storeList, storeIndex);
  if (store)
  {
    player->pc_SellItemStore(
      store,
      sellNum,
      reinterpret_cast<_sell_store_request_clzo::_list *>(pBuf + 9),
      *reinterpret_cast<unsigned int *>(pBuf + 5));
  }
  return true;
}

bool CNetworkEX::StoreListRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  player->SendMsg_StoreListResult();
  return true;
}

bool CNetworkEX::ExchangeDalantForGoldRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }
  player->pc_ExchangeDalantForGold(*reinterpret_cast<unsigned int *>(pBuf));
  return true;
}

bool CNetworkEX::ExchangeGoldForDalantRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }
  player->pc_ExchangeGoldForDalant(*reinterpret_cast<unsigned int *>(pBuf));
  return true;
}

bool CNetworkEX::LimitItemNumRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }
  player->pc_LimitItemNumRequest(*reinterpret_cast<unsigned int *>(pBuf));
  return true;
}

bool CNetworkEX::TalikRecorverList(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }
  CPvpCashPoint::SendMsg_TalikList(&player->m_kPvpCashPoint, n);
  return true;
}

bool CNetworkEX::PvpCashRecorverWithTalik(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }
  player->pc_PvpCashRecorver(*reinterpret_cast<unsigned int *>(pBuf), static_cast<unsigned __int8>(pBuf[4]));
  return true;
}

bool CNetworkEX::PcBangPrimiumCouponRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }
  CCouponMgr::ReceivePrimiumCoupon(&player->m_kPcBangCoupon, n);
  return true;
}

bool CNetworkEX::AttackPersonalRequest(unsigned int n, char *pBuf)
{
  char *packet = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *target = reinterpret_cast<CCharacter *>(
    CMainThread::GetObjectA(&g_Main, 0, static_cast<unsigned __int8>(*packet), *reinterpret_cast<unsigned __int16 *>(packet + 1)));
  if (target)
  {
    if (static_cast<unsigned __int8>(packet[3]) < 5u)
    {
      player->pc_PlayAttack_Gen(
        target,
        static_cast<unsigned __int8>(packet[3]),
        *reinterpret_cast<unsigned __int16 *>(packet + 4),
        *reinterpret_cast<unsigned __int16 *>(packet + 6),
        false);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: AttackPersonalRequest()..  if(pRecv->byAttPart >= base_fix_num)",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(&m_LogFile, "odd.. %s: AttackPersonalRequest()..  if(!pDst)", charName);
  return false;
}

bool CNetworkEX::AttackSkillRequest(unsigned int n, char *pBuf)
{
  char *packet = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *target = reinterpret_cast<CCharacter *>(
    CMainThread::GetObjectA(&g_Main, 0, static_cast<unsigned __int8>(*packet), *reinterpret_cast<unsigned __int16 *>(packet + 1)));
  const unsigned __int8 effectCode = static_cast<unsigned __int8>(packet[3]);
  const bool effectCodeOutOfRange = effectCode > 4u;
  if (!effectCodeOutOfRange || effectCode == 2 || effectCode == 3)
  {
    if (CRecordData::GetRecord(&g_Main.m_tblEffectData[effectCode], *reinterpret_cast<unsigned __int16 *>(packet + 4)))
    {
      float attackPos[3]{};
      attackPos[0] = static_cast<float>(*reinterpret_cast<short *>(packet + 8));
      attackPos[1] = 0.0f;
      attackPos[2] = static_cast<float>(*reinterpret_cast<short *>(packet + 10));
      player->pc_PlayAttack_Skill(
        target,
        attackPos,
        effectCode,
        *reinterpret_cast<unsigned __int16 *>(packet + 4),
        *reinterpret_cast<unsigned __int16 *>(packet + 6),
        reinterpret_cast<unsigned __int16 *>(packet + 12),
        *reinterpret_cast<unsigned __int16 *>(packet + 18));
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: AttackSkillRequest()..  if(!g_Main.m_tblEffectData[pRecv->byEffectCode].GetRecord(pRecv->wSkillIndex))",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: AttackSkillRequest()..  if(pRecv->byEffectCode != effect_code_skill && pRecv->byEffectCode != effect_code_class)",
    charName);
  return false;
}

bool CNetworkEX::AttackForceRequest(unsigned int n, char *pBuf)
{
  char *packet = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *target = reinterpret_cast<CCharacter *>(
    CMainThread::GetObjectA(&g_Main, 0, static_cast<unsigned __int8>(*packet), *reinterpret_cast<unsigned __int16 *>(packet + 1)));
  if (target || (static_cast<unsigned __int8>(*packet) == 0xFF && *reinterpret_cast<unsigned __int16 *>(packet + 1) == 0xFFFF))
  {
    float areaPos[3]{};
    areaPos[0] = static_cast<float>(*reinterpret_cast<short *>(packet + 3));
    areaPos[1] = static_cast<float>(*reinterpret_cast<short *>(packet + 5));
    areaPos[2] = 0.0f;
    player->pc_PlayAttack_Force(
      target,
      areaPos,
      *reinterpret_cast<unsigned __int16 *>(packet + 7),
      reinterpret_cast<unsigned __int16 *>(packet + 9),
      *reinterpret_cast<unsigned __int16 *>(packet + 15));
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: AttackForceRequest()..  if(pRecv->byID != 0xFF || pRecv->wIndex != 0xFFFF)",
    charName);
  return false;
}

bool CNetworkEX::AttackUnitRequest(unsigned int n, char *pBuf)
{
  char *packet = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *target = reinterpret_cast<CCharacter *>(
    CMainThread::GetObjectA(&g_Main, 0, static_cast<unsigned __int8>(*packet), *reinterpret_cast<unsigned __int16 *>(packet + 1)));
  if (target)
  {
    if (static_cast<unsigned __int8>(packet[3]) < 2u)
    {
      player->pc_PlayAttack_Unit(target, static_cast<unsigned __int8>(packet[3]));
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: AttackUnitRequest()..  if(pRecv->byWeaponPart >= UNIT_BULLET_NUM)",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(&m_LogFile, "odd.. %s: AttackUnitRequest()..  if(!pDst)", charName);
  return false;
}

bool CNetworkEX::AttackTestRequest(unsigned int n, char *pBuf)
{
  char *packet = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int8 effectCode = static_cast<unsigned __int8>(*packet);
  if (effectCode == 0xFF || effectCode < 4u)
  {
    if (effectCode >= 4u
        || CRecordData::GetRecord(&g_Main.m_tblEffectData[effectCode], static_cast<unsigned __int8>(packet[1])))
    {
      const unsigned __int8 weaponPart = static_cast<unsigned __int8>(packet[4]);
      if (weaponPart == 0xFF || weaponPart < 2u)
      {
        return true;
      }

      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: AttackTestRequest()..  if(pRecv->byWeaponPart >= UNIT_BULLET_NUM)",
        charName);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: AttackTestRequest()..  if(!g_Main.m_tblEffectData[pRecv->byEffectCode].GetRecord(pRecv->byEffectIndex))",
      charName);
    return true;
  }

  const int effectCodeValue = static_cast<unsigned __int8>(*packet);
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: AttackTestRequest()..  if(pRecv->byEffectCode != 0xFF && pRecv->byEffectCode >= EFFECT_CODE_NUM) : %d",
    charName,
    effectCodeValue);
  return true;
}

bool CNetworkEX::AttackSiegeRequest(unsigned int n, char *pBuf)
{
  char *packet = pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (static_cast<unsigned __int8>(packet[7]) < 5u)
  {
    if (!player->IsActingSiegeMode())
    {
      auto *target = reinterpret_cast<CCharacter *>(
        CMainThread::GetObjectA(&g_Main, 0, static_cast<unsigned __int8>(*packet), *reinterpret_cast<unsigned __int16 *>(packet + 1)));
      float attackPos[3]{};
      attackPos[0] = static_cast<float>(*reinterpret_cast<short *>(packet + 3));
      attackPos[1] = static_cast<float>(*reinterpret_cast<short *>(packet + 5));
      attackPos[2] = 0.0f;
      player->pc_PlayAttack_Siege(
        target,
        attackPos,
        static_cast<unsigned __int8>(packet[7]),
        *reinterpret_cast<unsigned __int16 *>(packet + 8),
        *reinterpret_cast<unsigned __int16 *>(packet + 10));
    }
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: AttackSiegeRequest()..  if(pRecv->byAttPart >= base_fix_num)",
    charName);
  return false;
}

bool CNetworkEX::ItemboxTakeRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int16 boxIndex = *reinterpret_cast<unsigned __int16 *>(pBuf);
  if (boxIndex < MAX_ITEMBOX)
  {
    if (!_effect_parameter::GetEff_State(&player->m_EP, 26))
    {
      const unsigned __int16 addSerial = *reinterpret_cast<unsigned __int16 *>(pBuf + 2);
      player->pc_TakeGroundingItem(&g_ItemBox[boxIndex], addSerial);
    }
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ItemboxTakeRequest()..  if(pRecv->wItemBoxIndex >= MAX_ITEMBOX)",
    charName);
  return false;
}

bool CNetworkEX::ThrowStorageRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  auto *item = reinterpret_cast<_STORAGE_POS_INDIV *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (!item->byStorageCode || item->byStorageCode == 1 || item->byStorageCode == 2)
  {
    if (!_effect_parameter::GetEff_State(&player->m_EP, 26))
    {
      player->pc_ThrowStorageItem(item);
    }
    return true;
  }

  const int storageCode = item->byStorageCode;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ThrowStorageRequest() : pRecv->Item.byStorageCode(%d)",
    charName,
    storageCode);
  return false;
}

bool CNetworkEX::UsePotionRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return true;
  }

  auto *packet = reinterpret_cast<unsigned __int8 *>(pBuf);
  if (packet[7])
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: UsePotionRequest()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  if (packet[0])
  {
    return true;
  }

  auto *targetPlayer = reinterpret_cast<CPlayer *>(
    CMainThread::GetObjectA(&g_Main, 0, 0, *reinterpret_cast<unsigned __int16 *>(packet + 1)));
  if (targetPlayer)
  {
    if (targetPlayer->m_bOper)
    {
      player->pc_UsePotionItem(targetPlayer, reinterpret_cast<_STORAGE_POS_INDIV *>(packet + 7));
    }
  }
  return true;
}

bool CNetworkEX::EquipPartRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *item = reinterpret_cast<_STORAGE_POS_INDIV *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (item->byStorageCode)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
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
  auto *item = reinterpret_cast<_STORAGE_POS_INDIV *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (item->byStorageCode)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: EmbellishRequest()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  const unsigned __int16 changeSerial = *reinterpret_cast<unsigned __int16 *>(&item[1].byStorageCode);
  player->pc_EmbellishPart(item, changeSerial);
  return true;
}

bool CNetworkEX::OffPartRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *item = reinterpret_cast<_STORAGE_POS_INDIV *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (item->byStorageCode == 1 || item->byStorageCode == 2)
  {
    player->pc_OffPart(item);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
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
    const int recordNum = CRecordData::GetRecordNum(&g_Main.m_tblItemMakeData);
    if (manualIndex < recordNum)
    {
      player->pc_MakeItem(
        &request->ipMakeTool,
        request->wManualIndex,
        request->byMaterialNum,
        request->ipMaterials);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: MakeItemRequest()..  if(pRecv->wManualIndex >= g_Main.m_tblItemMakeData.GetRecordNum())",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: MakeItemRequest()..  if(pRecv->byMaterialNum > _make_item_request_clzo::material_num)",
    charName);
  return false;
}

bool CNetworkEX::UpgradeItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *packet = reinterpret_cast<_STORAGE_POS_INDIV *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int8 jewelNum = packet[3].byStorageCode;
  if (jewelNum <= 4u)
  {
    if (packet[1].byStorageCode)
    {
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: UpgradeItemRequest()..  if(pRecv->m_posTalik.byStorageCode != _STORAGE_POS::INVEN)",
        charName);
      return false;
    }

    for (int j = 0; j < jewelNum; ++j)
    {
      if (static_cast<unsigned __int8>(packet[j + 3].wItemSerial))
      {
        const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
        CLogFile::Write(
          &m_LogFile,
          "odd.. %s: UpgradeItemRequest()..  if(pRecv->m_posUpgJewel[i].byStorageCode != _STORAGE_POS::INVEN)",
          charName);
        return false;
      }
    }

    if (packet[2].byStorageCode)
    {
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: UpgradeItemRequest()..  if(pRecv->m_posToolItem.byStorageCode != _STORAGE_POS::INVEN)",
        charName);
      return false;
    }

    if (!packet->byStorageCode || packet->byStorageCode == 1)
    {
      player->pc_UpgradeItem(
        packet + 1,
        packet + 2,
        packet,
        jewelNum,
        reinterpret_cast<_STORAGE_POS_INDIV *>(reinterpret_cast<char *>(packet) + 13));
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: UpgradeItemRequest()..  if(pRecv->m_posToolItem.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: UpgradeItemRequest()..  if(pRecv->byJewelNum > upgrade_jewel_num)",
    charName);
  return false;
}

bool CNetworkEX::DownGradeItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *packet = reinterpret_cast<_STORAGE_POS_INDIV *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (packet[1].byStorageCode)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: DownGradeItemRequest()..  if(pRecv->m_posTalik.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  if (packet[2].byStorageCode)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: DownGradeItemRequest()..  if(pRecv->m_posToolItem.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  if (!packet->byStorageCode || packet->byStorageCode == 1)
  {
    player->pc_DowngradeItem(packet + 1, packet + 2, packet);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: DownGradeItemRequest()..  if(pRecv->m_posUpgItem.byStorageCode != _STORAGE_POS::INVEN && pRecv->m_posUpgItem.byStorageCode != _STORAGE_POS::EQUIP)",
    charName);
  return false;
}

bool CNetworkEX::AddBagRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int16 bagSerial = *reinterpret_cast<unsigned __int16 *>(pBuf);
  player->pc_AddBag(bagSerial);
  return true;
}

bool CNetworkEX::UseRecoverLossExpItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  if (player->m_bOper)
  {
    const unsigned __int16 itemSerial = *reinterpret_cast<unsigned __int16 *>(pBuf);
    const char result = player->pc_UseRecoverLossExpItem(itemSerial);
    char payload[1]{result};
    unsigned __int8 type[2]{7, 27};
    CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], player->m_ObjID.m_wIndex, type, payload, 1u);
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
    const int recordNum = CRecordData::GetRecordNum(&g_Main.m_tblItemCombineData);
    if (manualIndex < recordNum)
    {
      player->pc_CombineItem(
        request->wManualIndex,
        request->byMaterialNum,
        request->ipMaterials,
        request->wOverlapItemSerial);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: CombineItemRequest()..  if(pRecv->wManualIndex >= g_Main.m_tblItemCombineData.GetRecordNum())",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: CombineItemRequest()..  if(pRecv->byMaterialNum > _combine_item_request_clzo::material_num)",
    charName);
  return false;
}

bool CNetworkEX::ExchangeItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *packet = reinterpret_cast<unsigned __int16 *>(pBuf);
  const int manualIndex = packet[0];
  const int recordNum = CRecordData::GetRecordNum(&g_Main.m_tblItemExchangeData);
  if (manualIndex < recordNum)
  {
    player->pc_ExchangeItem(packet[0], packet[1]);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
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
    const unsigned int recordNum = CRecordData::GetRecordNum(ItemCombineMgr::ms_tbl_ItemCombine);
    if (request->wManualIndex < recordNum)
    {
      player->pc_CombineItemEx(request);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: CombineItemRequest()..  if(pRecv->wManualIndex >= ItemCombineMgr::ms_tbl_ItemCombine[eCOMMON].GetRecordNum()))",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
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

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(&m_LogFile, "odd.. %s: CombineExItemAcceptRequest().. \tIsNotDlgType  ", charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: CombineExItemAcceptRequest().. \tif(\tpRecv->SelectItemBuff.bySelectNum >= _combine_ex_item_accept_request_clzo::eMaxSelectItemNum ) ",
    charName);
  return false;
}

bool CNetworkEX::UseFireCrackerItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int16 itemSerial = *reinterpret_cast<unsigned __int16 *>(pBuf);
  const int result = player->pc_UseFireCracker(itemSerial);
  const int errorCode = (result < 0) ? result : 0;

  unsigned __int8 sendMsg[3]{};
  sendMsg[0] = static_cast<unsigned __int8>(errorCode);
  *reinterpret_cast<unsigned __int16 *>(sendMsg + 1) = itemSerial;
  unsigned __int8 type[2]{7, 38};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], player->m_ObjID.m_wIndex, type, sendMsg, 3u);

  if (result >= 0)
  {
    unsigned __int8 circleMsg[6]{};
    *reinterpret_cast<unsigned int *>(circleMsg) = player->m_dwObjSerial;
    *reinterpret_cast<unsigned __int16 *>(circleMsg + 4) = static_cast<unsigned __int16>(result);
    unsigned __int8 circleType[2]{7, 39};
    CGameObject::CircleReport(player, circleType, reinterpret_cast<char *>(circleMsg), 6, 0);
  }
  return true;
}

bool CNetworkEX::SetItemCheckRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  if (player->m_bOper)
  {
    auto *packet = reinterpret_cast<unsigned __int8 *>(pBuf);
    player->pc_SetItemCheckRequest(
      *reinterpret_cast<unsigned int *>(packet + 1),
      packet[5],
      packet[6],
      packet[0]);
  }
  return true;
}

bool CNetworkEX::UseSoccerBallItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned __int16 itemSerial = *reinterpret_cast<unsigned __int16 *>(pBuf);
  unsigned __int16 itemIndex[16]{};
  itemIndex[0] = 0xFFFFu;
  unsigned __int8 result = player->pc_UserSoccerBall(itemSerial, itemIndex);

  unsigned __int8 sendMsg[3]{};
  sendMsg[0] = result;
  *reinterpret_cast<unsigned __int16 *>(sendMsg + 1) = itemSerial;
  unsigned __int8 type[2]{7, 47};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], player->m_ObjID.m_wIndex, type, sendMsg, 3u);

  if (!result)
  {
    unsigned __int8 circleMsg[7]{};
    *reinterpret_cast<unsigned int *>(circleMsg) = player->m_dwObjSerial;
    *reinterpret_cast<unsigned __int16 *>(circleMsg + 4) = itemIndex[0];
    circleMsg[6] = static_cast<unsigned __int8>(player->m_bTakeSoccerBall);
    unsigned __int8 circleType[2]{7, 48};
    CGameObject::CircleReport(player, circleType, reinterpret_cast<char *>(circleMsg), 7, 0);
    player->SenseState();
  }
  return true;
}

bool CNetworkEX::UseRadarItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *item = reinterpret_cast<_STORAGE_POS_INDIV *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (item->byStorageCode)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: UseRadarRequest()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  player->pc_UseRadarItem(item, reinterpret_cast<unsigned __int16 *>(&item[1].byStorageCode));
  return true;
}

bool CNetworkEX::RadarCharListRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
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

bool CNetworkEX::UseRecallTeleportItemRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *item = reinterpret_cast<_STORAGE_POS_INDIV *>(pBuf);
  if (!player->m_bOper)
  {
    return true;
  }

  if (item->byStorageCode)
  {
    player->SendMsg_UsePotionResult(0x19u, item->wItemSerial, 0);
  }

  char targetName[17]{};
  strncpy_s(targetName, sizeof(targetName), reinterpret_cast<char *>(&item[1]), 0x10u);
  CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
  if (CNationSettingManager::IsNormalString(manager, targetName))
  {
    CPlayer *targetPlayer = GetPtrPlayerFromName(g_Player, MAX_PLAYER, targetName);
    if (targetPlayer)
    {
      player->pc_UsePotionItem(targetPlayer, item);
    }
    else
    {
      CRecallEffectController *recall = CRecallEffectController::Instance();
      CRecallEffectController::SendRecallReqeustResult(recall, 0x11u, player);
    }
    return true;
  }

  CRecallEffectController *recall = CRecallEffectController::Instance();
  CRecallEffectController::SendRecallReqeustResult(recall, 0x11u, player);
  return true;
}

bool CNetworkEX::CharacterRenameCash(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *packet = reinterpret_cast<unsigned __int8 *>(pBuf);
  if (packet[1])
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: CharacterRenameCash()..  if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)",
      charName);
    return false;
  }

  char newName[17]{};
  strncpy_s(newName, sizeof(newName), reinterpret_cast<char *>(packet + 5), 0x10u);
  player->pc_CharacterRenameCash(
    packet[0],
    reinterpret_cast<_STORAGE_POS_INDIV *>(packet + 1),
    newName);
  return true;
}

bool CNetworkEX::TalikCrystalExchangeRequest(unsigned int n, _MSG_HEADER *pHeader, char *pBuf)
{
  (void)pHeader;
  auto *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int8 exchangeNum = static_cast<unsigned __int8>(*pBuf);
  if (exchangeNum <= 0x18u)
  {
    if (exchangeNum)
    {
      player->pc_TalikCrystalExchange(exchangeNum, reinterpret_cast<_talik_crystal_exchange_clzo::_list *>(pBuf + 1));
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: TalikCrystalExchangeRequest() : if(pRecv->byExchangeNum == 0)",
      charName);
    return false;
  }

  const int exchangeNumValue = exchangeNum;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: TalikCrystalExchangeRequest() : pRecv->byExchangeNum(%d) >= _talik_crystal_exchange_clzo::exchange_zone(%d)",
    charName,
    exchangeNumValue,
    24);
  return false;
}

bool CNetworkEX::ForceInvenChangeRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_force_inven_change_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return true;
  }

  if (!request->Item.byStorageCode || request->Item.byStorageCode == 3)
  {
    player->pc_ForceInvenChange(&request->Item, request->wReplaceSerial);
    return true;
  }

  const int storageCode = request->Item.byStorageCode;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ForceInvenChangeRequest() : pRecv->Item.byStorageCode(%d)",
    charName,
    storageCode);
  return false;
}

bool CNetworkEX::AnimusInvenChangeRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_animus_inven_change_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return true;
  }

  if (!request->Item.byStorageCode || request->Item.byStorageCode == 4)
  {
    player->pc_AnimusInvenChange(&request->Item, request->wReplaceSerial);
    return true;
  }

  const int storageCode = request->Item.byStorageCode;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: AnimusInvenChangeRequest() : pRecv->Item.byStorageCode(%d)",
    charName,
    storageCode);
  return false;
}

bool CNetworkEX::ResSeparationRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_res_separation_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return true;
  }

  if (request->byMoveAmount < 0x63u)
  {
    player->pc_ResSeparation(request->wStartSerial, request->byMoveAmount);
    return true;
  }

  const int amount = request->byMoveAmount;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ResSeparationRequest() : pRecv->byMoveAmount(%d)",
    charName,
    amount);
  return false;
}

bool CNetworkEX::ResDivisionRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_res_division_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return true;
  }

  if (request->byMoveAmount < 0x63u)
  {
    player->pc_ResDivision(request->wStartSerial, request->wTarSerial, request->byMoveAmount);
    return true;
  }

  const int amount = request->byMoveAmount;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ResDivisionRequest() : pRecv->byMoveAmount(%d)",
    charName,
    amount);
  return false;
}

bool CNetworkEX::PotionSocketSeparationRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_potionsocket_separation_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return true;
  }

  if (request->byMoveAmount <= 0x63u)
  {
    player->pc_PotionSeparation(request->wStartSerial, request->byMoveAmount);
    return true;
  }

  const int amount = request->byMoveAmount;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: PotionSocketSeparationRequest() : pRecv->byMoveAmount(%d)",
    charName,
    amount);
  return false;
}

bool CNetworkEX::PotionSocketDivisionRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_potionsocket_division_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return true;
  }

  if (request->byMoveAmount <= 0x63u)
  {
    player->pc_PotionDivision(request->wStartSerial, request->wTarSerial, request->byMoveAmount);
    return true;
  }

  const int amount = request->byMoveAmount;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: PotionSocketDivisionRequest() : pRecv->byMoveAmount(%d)",
    charName,
    amount);
  return false;
}

bool CNetworkEX::AlterItemSlotRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_alter_item_slot_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return true;
  }

  if (request->byNum && request->byNum <= 0x64u)
  {
    for (int j = 0; j < request->byNum; ++j)
    {
      const auto &entry = request->list[j];
      if (entry.byStorageIndex)
      {
        if (entry.byStorageIndex != 2)
        {
          const int storageIndex = entry.byStorageIndex;
          const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
          CLogFile::Write(
            &m_LogFile,
            "odd.. %s: AlterItemSlotRequest() : pRecv->list[%d].byStorageIndex(%d)",
            charName,
            j,
            storageIndex);
          return false;
        }
        if (entry.byClientSlotIndex >= 7u)
        {
          const int clientIndex = entry.byClientSlotIndex;
          const int storageIndex = entry.byStorageIndex;
          const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
          CLogFile::Write(
            &m_LogFile,
            "odd.. %s: AlterItemSlotRequest() : pRecv->list[%d].byStorageIndex(%d), pRecv->list[%d].byClientSlotIndex(%d)",
            charName,
            j,
            storageIndex,
            j,
            clientIndex);
          return false;
        }
      }
      else if (entry.byClientSlotIndex >= 0x64u)
      {
        const int clientIndex = entry.byClientSlotIndex;
        const int storageIndex = entry.byStorageIndex;
        const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
        CLogFile::Write(
          &m_LogFile,
          "odd.. %s: AlterItemSlotRequest() : pRecv->list[%d].byStorageIndex(%d), pRecv->list[%d].byClientSlotIndex(%d)",
          charName,
          j,
          storageIndex,
          j,
          clientIndex);
        return false;
      }
    }
    player->pc_AlterItemSlotRequest(request->byNum, request->list);
    return true;
  }

  const int num = request->byNum;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: AlterItemSlotRequest() : pRecv->byNum(%d)",
    charName,
    num);
  return false;
}

bool CNetworkEX::AlterLinkBoardSlotRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_alter_link_slot_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return true;
  }

  if (request->byNum < 0x32u)
  {
    for (int j = 0; j < request->byNum; ++j)
    {
      const auto &entry = request->list[j];
      if (entry.bySlotIndex >= 0x32u)
      {
        const int slotIndex = entry.bySlotIndex;
        const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
        CLogFile::Write(
          &m_LogFile,
          "odd.. %s: AlterLinkBoardSlotRequest() : pRecv->list[%d].bySlotIndex(%d)",
          charName,
          j,
          slotIndex);
        return false;
      }
      if (entry.byLinkCode != 0xFFu && entry.byLinkCode >= 7u)
      {
        const int linkCode = entry.byLinkCode;
        const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
        CLogFile::Write(
          &m_LogFile,
          "odd.. %s: AlterLinkBoardSlotRequest() : pRecv->list[%d].byLinkCode(%d)",
          charName,
          j,
          linkCode);
        return false;
      }
    }
    player->pc_AlterLinkBoardSlotRequest(request->byNum, request->list, request->byLinkLock);
    return true;
  }

  const int num = request->byNum;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: AlterLinkBoardSlotRequest() : pRecv->byNum(%d)",
    charName,
    num);
  return false;
}

bool CNetworkEX::PvpRankListRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_pvp_rank_list_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return true;
  }

  if (request->byRace < 3u)
  {
    if (request->byPage < 0xAu)
    {
      CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
      ranking->PvpRankListRequest(
        player->m_ObjID.m_wIndex,
        request->byRace,
        request->byVersion,
        request->byPage);
      return true;
    }

    const int page = request->byPage;
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: PvpRankListRequest() : pRecv->byPage(%d)",
      charName,
      page);
    return false;
  }

  const int race = request->byRace;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: PvpRankListRequest() : pRecv->byRace(%d)",
    charName,
    race);
  return false;
}

bool CNetworkEX::ModeChangeRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_mode_change_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (!request->byModeCode || request->byModeCode == 1)
  {
    if (!request->byStandCode || request->byStandCode == 1)
    {
      player->pc_ChangeModeType(request->byModeCode, request->byStandCode);
      return true;
    }

    const int standCode = request->byStandCode;
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ModeChangeRequest() : pRecv->byStandCode(%d)",
      charName,
      standCode);
    return false;
  }

  const int modeCode = request->byModeCode;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ModeChangeRequest() : pRecv->byModeCode(%d)",
    charName,
    modeCode);
  return false;
}

bool CNetworkEX::GustureRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_gesture_request_clzo *>(pBuf);
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }

  if (request->byGestureType < 0x64u)
  {
    player->pc_GestureRequest(request->byGestureType);
    return true;
  }

  const int gestureType = request->byGestureType;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: GustureRequest() : pRecv->byGestureType(%d)",
    charName,
    gestureType);
  return false;
}

bool CNetworkEX::AlterWindowInfoRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
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
  return true;
}

bool CNetworkEX::SetTargetObjectRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_set_target_object_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    CGameObject *target = CMainThread::GetObjectA(
      &g_Main,
      request->byKind,
      request->byID,
      request->wIndex);
    player->pc_SetTargetObjectRequest(target, request->dwSerial, false);
  }
  return true;
}

bool CNetworkEX::ReleaseTargetObjectRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  auto *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_ReleaseTargetObjectRequest();
  }
  return true;
}

bool CNetworkEX::PartyReqBlockReport(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<bool *>(pBuf);
  if (player->m_bOper)
  {
    player->pc_PartyReqBlock(*request);
  }
  return true;
}

bool CNetworkEX::WhisperBlockReport(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<bool *>(pBuf);
  if (player->m_bOper)
  {
    player->pc_WhisperBlock(*request);
  }
  return true;
}

bool CNetworkEX::TradeBlockReport(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<bool *>(pBuf);
  if (player->m_bOper)
  {
    player->pc_TradeBlock(*request);
  }
  return true;
}

bool CNetworkEX::GuildBattleBlockReport(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<bool *>(pBuf);
  if (player->m_bOper)
  {
    player->pc_GuildBattleBlock(*request);
  }
  return true;
}

bool CNetworkEX::PlayerMacroUpdate(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_MacroUpdate(pBuf);
  }
  return true;
}

bool CNetworkEX::TotalGuildRankRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_total_guild_rank_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    CTotalGuildRankManager *manager = CTotalGuildRankManager::Instance();
    manager->Send(request->dwVer, request->byRace, player);
  }
  return true;
}

bool CNetworkEX::WeeklyGuildRankRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_weekly_guild_rank_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    CWeeklyGuildRankManager *manager = CWeeklyGuildRankManager::Instance();
    manager->Send(request->dwVer, request->byRace, player);
  }
  return true;
}

bool CNetworkEX::SetRaceBossCryMsgRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_set_raceboss_cry_msg_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return true;
  }

  if (request->bySlot >= 0xAu)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: SetRaceBossCryMsgRequest()..  if( pRecv->bySlot >= max_cry_msg || pRecv->bySlot < 0)",
      charName);
    return false;
  }

  char cryMsg[65]{};
  strncpy_s(cryMsg, request->wszCryMsg, 0x40u);
  player->pc_SetRaceBossCryMsg(request->bySlot, cryMsg);
  return true;
}

bool CNetworkEX::NotifyRaceBossCryMsg(unsigned int n, char *pBuf)
{
  (void)pBuf;
  auto *player = &g_Player[n];
  player->pc_NotifyRaceBossCryMsg();
  return true;
}

bool CNetworkEX::SetGroupTargetObjectRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_set_group_target_object_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return true;
  }

  if (request->byGroupType < 3u)
  {
    CGameObject *target = CMainThread::GetObjectA(&g_Main, 0, request->byID, request->wIndex);
    player->pc_SetGroupTargetObjectRequest(target, request->dwSerial, request->byGroupType);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: SetGroupTargetObjectRequest()..  if(pRecv->byGroupType < 0 || pRecv->byGroupType >= GROUP_TYPE_NUM)",
    charName);
  return false;
}

bool CNetworkEX::ReleaseGroupTargetObjectRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_release_group_target_object_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return true;
  }

  if (request->byGroupType < 3u)
  {
    player->pc_ReleaseGroupTargetObjectRequest(request->byGroupType);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ReleaseGroupTargetObjectRequest()..  if(pRecv->byGroupType < 0 || pRecv->byGroupType >= GROUP_TYPE_NUM)",
    charName);
  return false;
}

bool CNetworkEX::SetGroupMapPointRequest(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_set_group_map_point_request_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return true;
  }

  if (request->byGroupType < 3u)
  {
    player->pc_SetGroupMapPointRequest(request->byGroupType, request->zPos);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: SetGroupMapPointRequest()..  if(pRecv->byGroupType < 0 || pRecv->byGroupType >= GROUP_TYPE_NUM)",
    charName);
  return false;
}

bool CNetworkEX::RequestPatriarchPunishment(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_pt_request_punishment_clzo *>(pBuf);
  if (!player->m_bOper)
  {
    return true;
  }

  if (request->byType < 3u)
  {
    if (request->wContentSize <= 0x4FFu)
    {
      if (request->wContentSize)
      {
        char content[1296]{};
        memcpy_0(content, request->wszContent, request->wContentSize);
        content[request->wContentSize] = 0;
        player->pc_RequestPatriarchPunishment(request->byType, request->wszCharName, content);
        return true;
      }

      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: RequestPatriarchPunishment()..  if (pRecv->wContentSize == 0)",
        charName);
      return false;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: RequestPatriarchPunishment()..  if (pRecv->wContentSize > _pt_request_punishment_clzo::content_size-1)",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: RequestPatriarchPunishment()..  if (pRecv->byType < 0 || pRecv->byType >= PUNISHMENT_TYPE_NUM)",
    charName);
  return false;
}

bool CNetworkEX::RequestTaxRate(unsigned int n, char *pBuf)
{
  (void)pBuf;
  auto *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_RequestTaxRate();
  }
  return true;
}

bool CNetworkEX::RequestChangeTaxRate(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<unsigned __int8 *>(pBuf);
  if (player->m_bOper)
  {
    player->pc_RequestChangeTaxRate(*request);
  }
  return true;
}

bool CNetworkEX::RequestUILockInit(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *user = &g_UserDB[n];
  auto *request = reinterpret_cast<_uilock_init_request_clzo *>(pBuf);

  char uilockPw[13]{};
  char uilockPwConfirm[13]{};
  char hintAnswer[17]{};
  memcpy_0(uilockPw, request->uszUILockPW, 12);
  memcpy_0(uilockPwConfirm, request->uszUILockPW_Confirm, 12);
  memcpy_0(hintAnswer, request->uszUILock_HintAnswer, 16);

  player->pc_RequestUILockInit(
    user,
    uilockPw,
    uilockPwConfirm,
    request->byUILock_HintIndex,
    hintAnswer);
  return true;
}

bool CNetworkEX::RequestUILockUserCertify(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *user = &g_UserDB[n];
  auto *request = reinterpret_cast<_uilock_user_certify_request_clzo *>(pBuf);
  player->pc_RequestUILockCertify(user, request->szUILockPW);
  return true;
}

bool CNetworkEX::RequestUILockUpdateInfo(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *request = reinterpret_cast<_uilock_update_info_request_clzo *>(pBuf);
  if (player->m_bOper)
  {
    player->pc_RequestUILockUpdate(
      request->uszUILockPW_Old,
      request->uszUILockPW,
      request->uszUILockPW_Confirm,
      request->byHintIndex,
      request->uszHintAnswer);
  }
  return true;
}

bool CNetworkEX::RequestUILockFindPW(unsigned int n, char *pBuf)
{
  auto *player = &g_Player[n];
  auto *user = &g_UserDB[n];
  auto *request = reinterpret_cast<_uilock_find_pw_request_clzo *>(pBuf);
  player->pc_RequestUILockFindPW(user, request->uszHintAnswer);
  return true;
}

bool CNetworkEX::RequestTLLogoutTime(unsigned int n, char *pBuf)
{
  (void)pBuf;
  auto *user = &g_UserDB[n];
  if (!user)
  {
    return false;
  }

  _qry_case_select_timelimit_info query;
  query.dwAccountSerial = user->m_dwAccountSerial;
  query.wIndex = static_cast<unsigned __int16>(n);
  const int size = query.size();
  CMainThread::PushDQSData(&g_Main, 0xFFFFFFFF, nullptr, 0x99u, reinterpret_cast<char *>(&query), size);
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

bool CNetworkEX::MineStartRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *recv = reinterpret_cast<_mine_start_request_clzo *>(pBuf);
  if (recv->byMineIndex < player->m_pCurMap->m_nResDumNum)
  {
    const int oreIndex = recv->byOreIndex;
    const int recordNum = CRecordData::GetRecordNum(&g_Main.m_tblItemData[17]);
    if (oreIndex < recordNum)
    {
      if (player->m_pCurMap)
      {
        player->pc_MineStart(recv->byMineIndex, recv->byOreIndex, recv->wBatterySerial);
        return true;
      }

      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      m_LogFile.Write("odd.. %s: MineStartRequest() .. if(!pOne->m_pCurMap)", charName);
      return false;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    m_LogFile.Write(
      "odd.. %s: if(pRecv->byOreIndex >= g_Main.m_tblItemData[tbl_code_ore].GetRecordNum())",
      charName);
    return false;
  }

  if (player->m_pCurMap->m_nResDumNum > 0)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    m_LogFile.Write(
      "odd.. %s: MineStartRequest() : pRecv->byMineIndex(%d) >= pOne->m_pCurMap->m_nResDumNum(%d)",
      charName,
      static_cast<int>(recv->byMineIndex),
      player->m_pCurMap->m_nResDumNum);
  }

  return true;
}

bool CNetworkEX::MineCancleRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  static_cast<void>(pBuf);
  player->pc_MineCancle();
  return true;
}

bool CNetworkEX::ResCuttingRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *recv = reinterpret_cast<_ore_cutting_request_clzo *>(pBuf);
  if (recv->byProcessNum <= 0x63u)
  {
    player->pc_OreCutting(recv->wOreSerial, recv->byProcessNum);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  m_LogFile.Write(
    "odd.. %s: ResCuttingRequest()..  if(pRecv->byProcessNum > max_overlap_num)",
    charName);
  return false;
}

bool CNetworkEX::OreIntoBagRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *recv = reinterpret_cast<_ore_into_bag_request_clzo *>(pBuf);
  if (recv->byAddAmount <= 0x63u)
  {
    const int recordNum = CRecordData::GetRecordNum(&g_Main.m_tblItemData[18]);
    if (recv->wResIndex < recordNum)
    {
      player->pc_OreIntoBag(recv->wResIndex, recv->wSerial, recv->byAddAmount);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    m_LogFile.Write(
      "odd.. %s: OreIntoBagRequest()..  if(pRecv->wResIndex >= g_Main.m_tblItemData[tbl_code_res].GetRecordNum())",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  m_LogFile.Write(
    "odd.. %s: OreIntoBagRequest()..  if(pRecv->byAddAmount > max_overlap_num)",
    charName);
  return false;
}

bool CNetworkEX::CuttingCompleteRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  static_cast<void>(pBuf);
  player->pc_CuttingComplete(0);
  return true;
}

bool CNetworkEX::PartyJoinInvitation(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (player->m_EP.GetEff_Have(50) <= 0.0f)
  {
    const unsigned __int16 dstIndex = *reinterpret_cast<unsigned __int16 *>(pBuf);
    if (dstIndex < MAX_PLAYER)
    {
      player->pc_PartyJoinInvitation(dstIndex);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: PartyJoinInvitation()..  if(pRecv->wDstIndex >= MAX_PLAYER)",
      charName);
    return false;
  }

  player->SendMsg_JadeEffectErr(1u);
  return true;
}

bool CNetworkEX::PartyJoinInvitationAnswer(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *bossId = reinterpret_cast<_CLID *>(pBuf);
  if (bossId->wIndex < MAX_PLAYER)
  {
    player->pc_PartyJoinInvitationAnswer(bossId);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: PartyJoinInvitationAnswer()..  if(pRecv->idBoss.wIndex >= MAX_PLAYER)",
    charName);
  return false;
}

bool CNetworkEX::PartyJoinApplication(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int16 bossIndex = *reinterpret_cast<unsigned __int16 *>(pBuf);
  if (bossIndex < MAX_PLAYER)
  {
    player->pc_PartyJoinApplication(bossIndex);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: PartyJoinApplication()..  if(pRecv->wBossIndex >= MAX_PLAYER)",
    charName);
  return false;
}

bool CNetworkEX::PartyJoinApplicatiohAnswer(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  auto *applicantId = reinterpret_cast<_CLID *>(pBuf);
  if (applicantId->wIndex < MAX_PLAYER)
  {
    player->pc_PartyJoinApplicationAnswer(applicantId);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: PartyJoinApplication()..  if(pRecv->idApplicant.wIndex >= MAX_PLAYER)",
    charName);
  return false;
}

bool CNetworkEX::PartyLeaveSelfRequest(int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  player->pc_PartyLeaveSelfReqeuest();
  return true;
}

bool CNetworkEX::PartyLeaveCompulsionRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned int exiterSerial = *reinterpret_cast<unsigned int *>(pBuf);
  player->pc_PartyLeaveCompulsionReqeuest(exiterSerial);
  return true;
}

bool CNetworkEX::PartyDisjointRequest(int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  player->pc_PartyDisJointReqeuest();
  return true;
}

bool CNetworkEX::PartySuccessionRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned int successorSerial = *reinterpret_cast<unsigned int *>(pBuf);
  player->pc_PartySuccessionReqeuest(successorSerial);
  return true;
}

bool CNetworkEX::PartyLockRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const bool lockMode = *reinterpret_cast<bool *>(pBuf);
  player->pc_PartyLockReqeuest(lockMode);
  return true;
}

bool CNetworkEX::AlterPartyLootShareRequest(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int8 lootShareMode = *reinterpret_cast<unsigned __int8 *>(pBuf);
  if (lootShareMode < 3u)
  {
    player->pc_PartyAlterLootShareReqeuest(lootShareMode);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: PartyJoinApplication()..  if(pRecv->byLootShareMode >= PARTY_ITEM_AUTHOR_TYPE_NUM)",
    charName);
  return false;
}

bool CNetworkEX::AwayPartyInvitation(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (strlen_0(pBuf))
  {
    if (strlen_0(pBuf) <= 0x10)
    {
      player->pc_AwaypartyInvitationRequest(pBuf);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: AwayPartyInvitation()..  if (__strlen(wszCharName) > max_name_len)",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: AwayPartyInvitation()..  if (__strlen(pRecv->wszCharName) == 0)",
    charName);
  return false;
}

bool CNetworkEX::AwayPartyInvitationAnswer(int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned __int8 retCode = *reinterpret_cast<unsigned __int8 *>(pBuf);
  auto *bossId = reinterpret_cast<_CLID *>(pBuf + 1);
  if (bossId->wIndex < MAX_PLAYER)
  {
    player->pc_AwayPartyJoinInvitationAnswer(bossId, retCode);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: AwayPartyInvitationAnswer()..  if(pRecv->idBoss.wIndex >= MAX_PLAYER)",
    charName);
  return false;
}

bool CNetworkEX::ForceRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_force_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (CMainThread::GetObjectA(&g_Main, 0, request->idDst.byID, request->idDst.wIndex))
  {
    player->pc_ForceRequest(request->wForceSerial, &request->idDst, request->wConsumeItemSerial);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ForceRequest()..  if(!g_Main.GetObject(obj_kind_char, pRecv->idDst.byID, pRecv->idDst.wIndex))",
    charName);
  return false;
}

bool CNetworkEX::SkillRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_skill_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (CMainThread::GetObjectA(&g_Main, 0, request->idDst.byID, request->idDst.wIndex))
  {
    const int skillIndex = request->bySkillIndex;
    const int recordNum = CRecordData::GetRecordNum(g_Main.m_tblEffectData);
    if (skillIndex < recordNum)
    {
      player->pc_SkillRequest(request->bySkillIndex, &request->idDst, request->wConsumeItemSerial);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ForceRequest()..  if(pRecv->bySkillIndex >= g_Main.m_tblEffectData[effect_code_skill].GetRecordNum())",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ForceRequest()..  if(!g_Main.GetObject(obj_kind_char, pRecv->idDst.byID, pRecv->idDst.wIndex))",
    charName);
  return false;
}

bool CNetworkEX::ClassSkillRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_class_skill_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (CMainThread::GetObjectA(&g_Main, 0, request->idDst.byID, request->idDst.wIndex))
  {
    const int skillIndex = request->wSkillIndex;
    const int recordNum = CRecordData::GetRecordNum(&g_Main.m_tblEffectData[2]);
    if (skillIndex < recordNum)
    {
      player->pc_ClassSkillRequest(request->wSkillIndex, &request->idDst, request->wConsumeItemSerial);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: ClassSkillRequest()..  if(pRecv->wSkillIndex >= g_Main.m_tblEffectData[effect_code_class].GetRecordNum())",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ClassSkillRequest()..  if(!g_Main.GetObject(obj_kind_char, pRecv->idDst.byID, pRecv->idDst.wIndex))",
    charName);
  return false;
}

bool CNetworkEX::MakeTowerRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_make_tower_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const int recordNum = CRecordData::GetRecordNum(&g_Main.m_tblEffectData[2]);
  if (request->wSkillIndex >= recordNum)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: MakeTowerRequest()..  if(pRecv->wSkillIndex >= g_Main.m_tblEffectData[effect_code_class].GetRecordNum())",
      charName);
    return false;
  }

  if (request->byMaterialNum > _make_tower_request_clzo::max_material)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: MakeTowerRequest()..  if(pRecv->byMaterialNum > _make_tower_request_clzo::max_material)",
      charName);
    return false;
  }

  for (int j = 0; j < request->byMaterialNum; ++j)
  {
    if (request->Material[j].byMaterSlotIndex >= 3u)
    {
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: MakeTowerRequest()..  if(pRecv->Material[i].byMaterSlotIndex >= max_tower_material)",
        charName);
      return false;
    }

    if (request->Material[j].byAmount > 0x63u)
    {
      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: MakeTowerRequest()..  if(pRecv->Material[i].byAmount > max_overlap_num)",
        charName);
      return false;
    }
  }

  float makePos[3]{};
  ShortToFloat(request->zPos, makePos, 3);
  player->pc_MakeTowerRequest(
    request->wSkillIndex,
    request->wTowerItemSerial,
    request->byMaterialNum,
    request->Material,
    makePos,
    request->wConsumeItemSerial);
  return true;
}

bool CNetworkEX::BackTowerRequest(unsigned int n, char *pBuf)
{
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned int towerObjSerial = *reinterpret_cast<unsigned int *>(pBuf);
  player->pc_BackTowerRequest(towerObjSerial);
  return true;
}

bool CNetworkEX::MakeTrapRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_make_trap_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  _STORAGE_LIST::_db_con *trapItem = nullptr;
  _base_fld *itemRecord = nullptr;

  trapItem = _STORAGE_LIST::GetPtrFromSerial(&player->m_Param.m_dbInven, request->wTrapItemSerial);
  if (trapItem)
  {
    itemRecord = CRecordData::GetRecord(&g_Main.m_tblItemData[26], trapItem->m_wItemIndex);
  }

  if (trapItem && itemRecord)
  {
    if (*reinterpret_cast<int *>(itemRecord[1].m_strCode) == 1)
    {
      CNuclearBombMgr *nuclearMgr = CNuclearBombMgr::Instance();
      CNuclearBombMgr::SendMsg_Result(nuclearMgr, n, 1u);
      return true;
    }

    if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
    {
      return true;
    }

    const int recordNum = CRecordData::GetRecordNum(&g_Main.m_tblEffectData[2]);
    if (request->wSkillIndex < recordNum)
    {
      float makePos[3]{};
      ShortToFloat(request->zPos, makePos, 3);
      player->pc_MakeTrapRequest(request->wSkillIndex, request->wTrapItemSerial, makePos, request->wConsumeItemSerial);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: MakeTrapRequest()..  if(pRecv->wSkillIndex >= g_Main.m_tblEffectData[effect_code_class].GetRecordNum())",
      charName);
    return false;
  }

  CNuclearBombMgr *nuclearMgr = CNuclearBombMgr::Instance();
  CNuclearBombMgr::SendMsg_Result(nuclearMgr, n, 0xBu);
  return false;
}

bool CNetworkEX::DecideRecallRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_decide_recall_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  CRecallEffectController *recall = CRecallEffectController::Instance();
  CRecallEffectController::DecideRecall(recall, request->wRequestID, request->byAgree, player);
  return true;
}

bool CNetworkEX::BackTrapRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_back_trap_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  player->pc_BackTrapRequest(request->dwTrapObjSerial, request->wAddSerial);
  return true;
}

bool CNetworkEX::ForceRecallTeleportRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_force_recall_teleport_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  CPlayer *targetPlayer = GetPtrPlayerFromName(g_Player, MAX_PLAYER, request->wszRecallName);
  if (targetPlayer)
  {
    _CHRID targetId{};
    targetId.byID = targetPlayer->m_ObjID.m_byID;
    targetId.dwSerial = targetPlayer->m_ObjID.m_wIndex;
    targetId.wIndex = static_cast<unsigned __int16>(targetPlayer->m_dwObjSerial);
    player->pc_ForceRequest(request->wForceSerial, &targetId, request->wConsumeItemSerial);
  }
  else
  {
    CRecallEffectController *recall = CRecallEffectController::Instance();
    CRecallEffectController::SendRecallReqeustResult(recall, 0x11u, player);
  }
  return true;
}

bool CNetworkEX::SkillRecallTeleportRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_skill_recall_teleport_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const int recordNum = CRecordData::GetRecordNum(g_Main.m_tblEffectData);
  if (request->wSkillIndex < recordNum)
  {
    CPlayer *targetPlayer = GetPtrPlayerFromName(g_Player, MAX_PLAYER, request->wszRecallName);
    if (targetPlayer)
    {
      _CHRID targetId{};
      targetId.byID = targetPlayer->m_ObjID.m_byID;
      targetId.dwSerial = targetPlayer->m_ObjID.m_wIndex;
      targetId.wIndex = static_cast<unsigned __int16>(targetPlayer->m_dwObjSerial);
      player->pc_SkillRequest(static_cast<unsigned __int8>(request->wSkillIndex), &targetId, request->wConsumeItemSerial);
    }
    else
    {
      CRecallEffectController *recall = CRecallEffectController::Instance();
      CRecallEffectController::SendRecallReqeustResult(recall, 0x11u, player);
    }
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: SkillRecallTeleportRequest()..  if(pRecv->wSkillIndex >= g_Main.m_tblEffectData[effect_code_skill].GetRecordNum())",
    charName);
  return false;
}

bool CNetworkEX::ClassSkillRecallTeleportRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_class_skill_recall_teleport_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const int recordNum = CRecordData::GetRecordNum(g_Main.m_tblEffectData);
  if (request->wSkillIndex < recordNum)
  {
    CPlayer *targetPlayer = GetPtrPlayerFromName(g_Player, MAX_PLAYER, request->wszRecallName);
    if (targetPlayer)
    {
      _CHRID targetId{};
      targetId.byID = targetPlayer->m_ObjID.m_byID;
      targetId.dwSerial = targetPlayer->m_ObjID.m_wIndex;
      targetId.wIndex = static_cast<unsigned __int16>(targetPlayer->m_dwObjSerial);
      player->pc_ClassSkillRequest(request->wSkillIndex, &targetId, request->wConsumeItemSerial);
    }
    else
    {
      CRecallEffectController *recall = CRecallEffectController::Instance();
      CRecallEffectController::SendRecallReqeustResult(recall, 0x11u, player);
    }
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ClassSkillRecallTeleportRequest()..  if(pRecv->wSkillIndex >= g_Main.m_tblEffectData[effect_code_skill].GetRecordNum())",
    charName);
  return false;
}

bool CNetworkEX::ThrowSkillRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_thorw_skill_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (CMainThread::GetObjectA(&g_Main, 0, request->idDst.byID, request->idDst.wIndex))
  {
    player->pc_ThrowSkillRequest(request->wBulletSerial, &request->idDst, request->wConsumeItemSerial);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ThrowSkillRequest()..  if(!g_Main.GetObject(obj_kind_char, pRecv->idDst.byID, pRecv->idDst.wIndex))",
    charName);
  return false;
}

bool CNetworkEX::ThrowUnitRequest(unsigned int n, char *pBuf)
{
  auto *destId = reinterpret_cast<_CHRID *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (CMainThread::GetObjectA(&g_Main, 0, destId->byID, destId->wIndex))
  {
    auto *consumeSerial = &destId[1].wIndex;
    player->pc_ThrowUnitRequest(destId, consumeSerial);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: ThrowUnitRequest()..  if(!g_Main.GetObject(obj_kind_char, pRecv->idDst.byID, pRecv->idDst.wIndex))",
    charName);
  return false;
}

bool CNetworkEX::DTradeAskRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_d_trade_ask_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  CPlayer *target = &g_Player[request->wDstIndex];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (_effect_parameter::GetEff_Have(&player->m_EP, 50) > 0.0)
  {
    player->SendMsg_JadeEffectErr(3u);
    return true;
  }

  if (_effect_parameter::GetEff_State(&player->m_EP, 26) || _effect_parameter::GetEff_State(&target->m_EP, 26))
  {
    player->SendMsg_DTradeAskResult(0x20u);
    return true;
  }

  if (request->wDstIndex < MAX_PLAYER)
  {
    player->pc_DTradeAskRequest(request->wDstIndex);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: DTradeAskRequest().. if(pRecv->wDstIndex >= MAX_PLAYER)",
    charName);
  return false;
}

bool CNetworkEX::DTradeAnswerRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_d_trade_answer_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->wIndex < MAX_PLAYER)
  {
    _CLID askerId{};
    askerId.wIndex = request->wIndex;
    askerId.dwSerial = request->dwSerial;
    player->pc_DTradeAnswerRequest(&askerId);
    return true;
  }

  const int askerIndex = request->wIndex;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: DTradeAnswerRequest() : pRecv->idAsker.wIndex(%d) >= MAX_PLAYER(%d)",
    charName,
    askerIndex,
    MAX_PLAYER);
  return false;
}

bool CNetworkEX::DTradeCancleRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->pc_DTradeCancleRequest();
  }
  return true;
}

bool CNetworkEX::DTradeLockRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }

  player->pc_DTradeLockRequest();
  return true;
}

bool CNetworkEX::DTradeAddRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_d_trade_add_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }

  if (request->bySlotIndex <= 0xFu)
  {
    if (request->byStorageCode < 8u)
    {
      player->pc_DTradeAddRequest(
        request->bySlotIndex,
        request->byStorageCode,
        request->dwSerial,
        request->byAmount);
      return true;
    }

    const int storageCode = request->byStorageCode;
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: DTradeAddRequest() : pRecv->byStorageCode(%d) >= _STORAGE_POS::STORAGE_NUM(%d)",
      charName,
      storageCode,
      8);
    return false;
  }

  const int slotIndex = request->bySlotIndex;
  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: DTradeAddRequest() : pRecv->bySlotIndex(%d) > max_d_trade_item(%d)",
    charName,
    slotIndex,
    15);
  return false;
}

bool CNetworkEX::DTradeDelRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_d_trade_del_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }

  if (request->bySlotIndex <= 0xFu)
  {
    player->pc_DTradeDelRequest(request->bySlotIndex);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: DTradeDelRequest().. if(pRecv->bySlotIndex > max_d_trade_item)",
    charName);
  return false;
}

bool CNetworkEX::DTradeBetRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_d_trade_bet_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }

  if (request->byMoneyUnit == 0 || request->byMoneyUnit == 1)
  {
    player->pc_DTradeBetRequest(request->byMoneyUnit, request->dwBetAmount);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: DTradeBetRequest().. if(pRecv->byMoneyUnit != money_unit_dalant && pRecv->byMoneyUnit != money_unit_gold)",
    charName);
  return false;
}

bool CNetworkEX::DTradeOKRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_d_trade_ok_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }

  player->pc_DTradeOKRequest(request->dwKey);
  return true;
}

bool CNetworkEX::AnimusRecallRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_animus_recall_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  player->pc_AnimusRecallRequest(
    static_cast<unsigned __int16>(request->dwAnimusItemSerial),
    request->wAnimusClientHP,
    request->wAnimusClientFP);
  return true;
}

bool CNetworkEX::AnimusReturnRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  player->pc_AnimusReturnRequest();
  return true;
}

bool CNetworkEX::AnimusCommandRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_animus_command_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }

  if (request->byCommandCode < 2u)
  {
    player->pc_AnimusCommandRequest(request->byCommandCode);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: AnimusCommandRequest()..  if(pRecv->byCommandCode >= ANIMUS_ACT_TYPE_NUM)",
    charName);
  return false;
}

bool CNetworkEX::AnimusTargetRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_animus_target_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_bCorpse)
  {
    return true;
  }

  if (CMainThread::GetObjectA(&g_Main, 0, request->byObjectID, request->wObjectIndex))
  {
    player->pc_AnimusTargetRequest(request->byObjectID, request->wObjectIndex, request->dwObjectSerial);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: AnimusTargetRequest()..  if(!g_Main.GetObject(obj_kind_char, pRecv->byObjectID, pRecv->wObjectIndex))",
    charName);
  return false;
}

bool CNetworkEX::ObjectServerPosRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_object_server_pos_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }
  if (!player->m_byUserDgr)
  {
    return true;
  }

  unsigned __int8 resultCode = 0;
  CGameObject *object = CMainThread::GetObjectA(&g_Main, request->byKind, request->byID, request->wIndex);
  if (object)
  {
    if (object->m_bLive)
    {
      if (object->m_dwObjSerial != request->dwSerial)
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

  unsigned __int8 msg[0x15];
  msg[0] = resultCode;
  msg[1] = request->byKind;
  msg[2] = request->byID;
  memcpy_0(&msg[3], &request->wIndex, sizeof(request->wIndex));
  memcpy_0(&msg[5], &request->dwSerial, sizeof(request->dwSerial));
  if (resultCode == 0)
  {
    memcpy_0(&msg[9], object->m_fCurPos, 0xCuLL);
  }

  unsigned __int8 type[2];
  type[0] = 20;
  type[1] = 2;
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], n, type, reinterpret_cast<char *>(msg), 0x15u);
  return true;
}

bool CNetworkEX::UnitFrameBuyRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_unit_frame_buy_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->byFrameCode < 4u)
  {
    player->pc_UnitFrameBuyRequest(request->byFrameCode, request->bUseNPCLinkIntem);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: UnitFrameBuyRequest().. if(pRecv->byFrameCode >= UNIT_FRAME_CODE_NUM)",
    charName);
  return false;
}

bool CNetworkEX::UnitSellRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_unit_sell_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->bySlotIndex < 4u)
  {
    player->pc_UnitSellRequest(request->bySlotIndex, request->bUseNPCLinkIntem);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: UnitSellRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
    charName);
  return false;
}

bool CNetworkEX::UnitPartTuningRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_unit_part_tuning_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->bySlotIndex < 4u)
  {
    if (request->byTuningNum <= 6u)
    {
      for (int j = 0; j < request->byTuningNum; ++j)
      {
        if (request->TuningList[j].byPartCode >= 6u)
        {
          const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
          CLogFile::Write(
            &m_LogFile,
            "odd.. %s: UnitPartTuningRequest().. if(pRecv->TuningList[i].byPartCode >= UNIT_PART_NUM)",
            charName);
          return false;
        }
      }
      player->pc_UnitPartTuningRequest(
        request->bySlotIndex,
        request->byTuningNum,
        request->TuningList,
        request->bUseNPCLinkIntem);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: UnitPartTuningRequest().. if(pRecv->byTuningNum > UNIT_PART_NUM)",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: UnitPartTuningRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
    charName);
  return false;
}

bool CNetworkEX::UnitFrameRepairRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_unit_frame_repair_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->bySlotIndex < 4u)
  {
    player->pc_UnitFrameRepairRequest(request->bySlotIndex, request->bUseNPCLinkIntem);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: UnitFrameRepairRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
    charName);
  return false;
}

bool CNetworkEX::UnitBulletFillRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_unit_bullet_fill_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->bySlotIndex < 4u)
  {
    unsigned __int8 byFill = 0;
    for (int j = 0; j < 2; ++j)
    {
      if (request->wBulletIndex[j] != 0xFFFF)
      {
        if (!CRecordData::GetRecord(&g_Main.m_tblUnitBullet, request->wBulletIndex[j]))
        {
          const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
          CLogFile::Write(
            &m_LogFile,
            "odd.. %s: UnitBulletFillRequest().. if(!g_Main.m_tblUnitBullet.GetRecord(pRecv->wBulletIndex[i]))",
            charName);
          return false;
        }
        byFill |= static_cast<unsigned __int8>(1u << j);
      }
    }
    if (byFill)
    {
      player->pc_UnitBulletFillRequest(request->bySlotIndex, request->wBulletIndex, request->bUseNPCLinkIntem);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(&m_LogFile, "odd.. %s: UnitBulletFillRequest().. if(!byFill)", charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: UnitBulletFillRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
    charName);
  return false;
}

bool CNetworkEX::UnitPackFillRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_unit_pack_fill_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->bySlotIndex < 4u)
  {
    if (request->byFillNum <= 8u)
    {
      unsigned __int8 spareUsed[8]{};
      for (int j = 0; j < request->byFillNum; ++j)
      {
        if (request->List[j].bySpareIndex >= 8u)
        {
          const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
          CLogFile::Write(
            &m_LogFile,
            "odd.. %s: UnitBulletFillRequest().. if(pRecv->List[i].bySpareIndex >= max_unit_spare)",
            charName);
          return false;
        }
        if (!CRecordData::GetRecord(&g_Main.m_tblUnitBullet, request->List[j].wBulletIndex))
        {
          const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
          CLogFile::Write(
            &m_LogFile,
            "odd.. %s: UnitBulletFillRequest().. if(pRecv->List[i].bySpareIndex >= max_unit_spare)",
            charName);
          return false;
        }
        if (spareUsed[request->List[j].bySpareIndex])
        {
          const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
          CLogFile::Write(
            &m_LogFile,
            "odd.. %s: UnitBulletFillRequest().. if(bSpare[pRecv->List[i].bySpareIndex])",
            charName);
          return false;
        }
        spareUsed[request->List[j].bySpareIndex] = 1;
      }

      player->pc_UnitPackFillRequest(
        request->bySlotIndex,
        request->byFillNum,
        request->List,
        request->bUseNPCLinkIntem);
      return true;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: UnitBulletFillRequest().. if(pRecv->byFillNum > max_unit_spare)",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: UnitBulletFillRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
    charName);
  return false;
}

bool CNetworkEX::UnitDeliveryRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_unit_delivery_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->bySlotIndex >= 4u)
  {
    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: UnitDeliveryRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
      charName);
    return false;
  }

  CItemStore *store = nullptr;
  if (request->dwStoreIndex != static_cast<unsigned int>(-1))
  {
    const int mapSerial = CMapData::GetMapCode(player->m_pCurMap);
    CItemStoreManager *storeManager = CItemStoreManager::Instance();
    CMapItemStoreList *storeList = CItemStoreManager::GetMapItemStoreListBySerial(storeManager, mapSerial);
    if (!storeList)
    {
      return true;
    }

    for (int j = 0; j < storeList->m_nItemStoreNum; ++j)
    {
      CItemStore *candidate = &storeList->m_ItemStore[j];
      if (candidate->m_pRec->m_dwIndex == request->dwStoreIndex)
      {
        store = candidate;
        break;
      }
    }

    if (!store)
    {
      return true;
    }
  }

  float newPos[3]{};
  ShortToFloat(request->zUnitNewPos, newPos, 3);
  player->pc_UnitDeliveryRequest(request->bySlotIndex, store, true, newPos, request->bUseNPCLinkIntem);
  return true;
}

bool CNetworkEX::UnitReturnRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  player->pc_UnitReturnRequest();
  return true;
}

bool CNetworkEX::UnitTakeRequest(unsigned int n, char *pBuf)
{
  (void)pBuf;
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  player->pc_UnitTakeRequest();
  return true;
}

bool CNetworkEX::UnitLeaveRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_unit_leave_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  float newPos[3]{};
  ShortToFloat(request->zPlayerNewPos, newPos, 3);
  player->pc_UnitLeaveRequest(newPos);
  return true;
}

bool CNetworkEX::UnitBulletReplaceRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_unit_bullet_replace_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  if (request->bySlotIndex < 4u)
  {
    if (request->byPackIndex < 8u)
    {
      if (request->byBulletPart < 2u)
      {
        player->pc_UnitBulletReplaceRequest(request->bySlotIndex, request->byPackIndex, request->byBulletPart);
        return true;
      }

      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: UnitBulletReplaceRequest().. if(pRecv->byBulletPart >= UNIT_BULLET_NUM)",
        charName);
      return false;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: UnitBulletReplaceRequest().. if(pRecv->byPackIndex >= max_unit_spare)",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: UnitBulletReplaceRequest().. if(pRecv->bySlotIndex >= unit_storage_num)",
    charName);
  return false;
}

bool CNetworkEX::SelectWaitedQuestReport(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_select_waited_quest_report_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return true;
  }

  if (request->bySelectQuestIndex < 5u)
  {
    player->pc_SelectQuestAfterHappenEvent(request->bySelectQuestIndex);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: SelectClassRequest()..  if(pRecv->bySelectRewardItem >= max_class_bns_item)",
    charName);
  return false;
}

bool CNetworkEX::QuestSelectRewardReport(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_quest_select_reward_repart_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode)
  {
    return true;
  }

  if (request->byQuestDBSlot < 0x1Eu)
  {
    if (request->bySelectItemSlotIndex == 0xFF || request->bySelectItemSlotIndex < 6u)
    {
      if (request->byLinkQuestIndex == 0xFF || request->byLinkQuestIndex < 5u)
      {
        if (request->bySelectItemSlotIndex == 0xFF && request->byLinkQuestIndex == 0xFF)
        {
          const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
          CLogFile::Write(
            &m_LogFile,
            "odd.. %s: QuestSelectRewardReport()..  if(pRecv->bySelectItemSlotIndex == 0xFF && pRecv->byLinkQuestIndex == 0xFF)",
            charName);
          return false;
        }

        player->pc_SelectQuestReward(
          request->byQuestDBSlot,
          request->bySelectItemSlotIndex,
          request->byLinkQuestIndex);
        return true;
      }

      const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
      CLogFile::Write(
        &m_LogFile,
        "odd.. %s: QuestSelectRewardReport()..  if(pRecv->byLinkQuestIndex >= link_quest_num)",
        charName);
      return false;
    }

    const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
    CLogFile::Write(
      &m_LogFile,
      "odd.. %s: QuestSelectRewardReport()..  if(pRecv->bySelectItemSlotIndex >= reward_item_num)",
      charName);
    return false;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: QuestSelectRewardReport()..  if(pRecv->byQuestDBSlot >= quest_storage_num)",
    charName);
  return false;
}

bool CNetworkEX::NPCDialogRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_npc_quest_list_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned int recordNum = CRecordData::GetRecordNum(&CItemStoreManager::Instance()->m_tblItemStore);
  if (request->dwStoreIndex >= recordNum)
  {
    return true;
  }

  const int mapCode = CMapData::GetMapCode(player->m_pCurMap);
  CMapItemStoreList *mapStores = CItemStoreManager::GetMapItemStoreListBySerial(CItemStoreManager::Instance(), mapCode);
  if (!mapStores)
  {
    return true;
  }

  CItemStore *store = CMapItemStoreList::GetItemStoreFromRecIndex(mapStores, request->dwStoreIndex);
  if (store)
  {
    player->pc_RequestDialogWithNPC(store);
  }
  return true;
}

bool CNetworkEX::NPCWatchingRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_npc_quest_list_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  const unsigned int recordNum = CRecordData::GetRecordNum(&CItemStoreManager::Instance()->m_tblItemStore);
  if (request->dwStoreIndex >= recordNum)
  {
    return true;
  }

  const int mapCode = CMapData::GetMapCode(player->m_pCurMap);
  CMapItemStoreList *mapStores = CItemStoreManager::GetMapItemStoreListBySerial(CItemStoreManager::Instance(), mapCode);
  if (!mapStores)
  {
    return true;
  }

  CItemStore *store = CMapItemStoreList::GetItemStoreFromRecIndex(mapStores, request->dwStoreIndex);
  if (store)
  {
    player->pc_RequestWatchingWithNPC(store);
  }
  return true;
}

bool CNetworkEX::BriefPassReport(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_brief_pass_report_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  if (request->bySlotIndex < 0x1Eu)
  {
    player->pc_BriefPass(request->bySlotIndex);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: BriefPassReport()..  if(pRecv->bySlotIndex >= quest_storage_num)",
    charName);
  return false;
}

bool CNetworkEX::NPCQuestRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_npc_quest_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned int recordNum = CRecordData::GetRecordNum(&CItemStoreManager::Instance()->m_tblItemStore);
  if (request->dwStoreIndex >= recordNum)
  {
    return true;
  }

  const int mapCode = CMapData::GetMapCode(player->m_pCurMap);
  CMapItemStoreList *mapStores = CItemStoreManager::GetMapItemStoreListBySerial(CItemStoreManager::Instance(), mapCode);
  if (!mapStores)
  {
    return true;
  }

  CItemStore *store = CMapItemStoreList::GetItemStoreFromRecIndex(mapStores, request->dwStoreIndex);
  if (store)
  {
    player->pc_RequestQuestFromNPC(store, request->dwNPCQuestIndex);
  }
  return true;
}

bool CNetworkEX::NPCQuestListRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_npc_quest_list_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return true;
  }

  const unsigned int recordNum = CRecordData::GetRecordNum(&CItemStoreManager::Instance()->m_tblItemStore);
  if (request->dwStoreIndex >= recordNum)
  {
    return true;
  }

  const int mapCode = CMapData::GetMapCode(player->m_pCurMap);
  CMapItemStoreList *mapStores = CItemStoreManager::GetMapItemStoreListBySerial(CItemStoreManager::Instance(), mapCode);
  if (!mapStores)
  {
    return true;
  }

  CItemStore *store = CMapItemStoreList::GetItemStoreFromRecIndex(mapStores, request->dwStoreIndex);
  if (store)
  {
    player->pc_RequestQuestListFromNPC(store);
  }
  return true;
}

bool CNetworkEX::QuestGiveupRequest(unsigned int n, char *pBuf)
{
  auto *request = reinterpret_cast<_quest_give_up_request_clzo *>(pBuf);
  CPlayer *player = &g_Player[n];
  if (!player->m_bOper)
  {
    return true;
  }

  if (request->byQuestDBSlot < 0x1Eu)
  {
    player->pc_QuestGiveupRequest(request->byQuestDBSlot);
    return true;
  }

  const char *charName = CPlayerDB::GetCharNameA(&player->m_Param);
  CLogFile::Write(
    &m_LogFile,
    "odd.. %s: QuestGiveupRequest()..  if(pRecv->byQuestDBSlot >= quest_storage_num)",
    charName);
  return false;
}
