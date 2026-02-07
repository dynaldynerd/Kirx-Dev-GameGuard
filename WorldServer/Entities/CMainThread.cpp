#include "pch.h"

#include "CMainThread.h"

#include <crtdbg.h>
#include <process.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>

#include "AutominePersonalMgr.h"
#include "CActionPointSystemMgr.h"
#include "CAsyncLogger.h"
#include "CAttack.h"
#include "CBillingManager.h"
#include "CBossMonsterScheduleSystem.h"
#include "CCashDBWorkManager.h"
#include "CExchangeEvent.h"
#include "CGoldenBoxItemMgr.h"
#include "CGuildBattleController.h"
#include "CGuildRoomSystem.h"
#include "CHonorGuild.h"
#include "CPotionMgr.h"
#include "CLuaScriptMgr.h"
#include "CLogTypeDBTaskManager.h"
#include "CMapOperation.h"
#include "CMonsterEventRespawn.h"
#include "CMonsterEventSet.h"
#include "CMoveMapLimitManager.h"
#include "CNetworkEX.h"
#include "CPcBangFavor.h"
#include "CPostSystemManager.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CRaceBuffManager.h"
#include "CTotalGuildRankManager.h"
#include "CUnmannedTraderController.h"
#include "CWeeklyGuildRankManager.h"
#include "CashItemRemoteStore.h"
#include "AmuletItem_fld.h"
#include "AnimusItem_fld.h"
#include "BagItem_fld.h"
#include "BattleDungeonItem_fld.h"
#include "BatteryItem_fld.h"
#include "BootyItem_fld.h"
#include "BoxItem_fld.h"
#include "BulletItem_fld.h"
#include "CloakItem_fld.h"
#include "CouponItem_fld.h"
#include "DfnEquipItem_fld.h"
#include "EventItem_fld.h"
#include "FIRECRACKER_fld.h"
#include "FaceItem_fld.h"
#include "ForceItem_fld.h"
#include "GuardTowerItem_fld.h"
#include "MakeToolItem_fld.h"
#include "MapItem_fld.h"
#include "mobmsg_fld.h"
#include "npc_fld.h"
#include "NPCLink_fld.h"
#include "OreItem_fld.h"
#include "player_fld.h"
#include "PotionItem_fld.h"
#include "RadarItem_fld.h"
#include "RecoveryItem_fld.h"
#include "ResourceItem_fld.h"
#include "RingItem_fld.h"
#include "SiegeKitItem_fld.h"
#include "TOWNItem_fld.h"
#include "TicketItem_fld.h"
#include "TrapItem_fld.h"
#include "UnitBullet_fld.h"
#include "UnitKeyItem_fld.h"
#include "UnitPart_fld.h"
#include "UNmannedminer_fld.h"
#include "WeaponItem_fld.h"
#include "CandidateMgr.h"
#include "CSUItemSystem.h"
#include "CDarkHoleDungeonQuest.h"
#include "CCryptor.h"
#include "CRtc.h"
#include "CNationCodeStrTable.h"
#include "CNationSettingManager.h"
#include "NameTxt_fld.h"
#include "ResourceItem_fld.h"
#include "UnitFrame_fld.h"
#include "UnitPart_fld.h"
#include "UnitBullet_fld.h"
#include "AnimusItem_fld.h"
#include "ItemMakeData_fld.h"
#include "EditData_fld.h"
#include "BulletItem_fld.h"
#include "CNuclearBombMgr.h"
#include "CPvpCashMng.h"
#include "CQuestMgr.h"
#include "CRecallEffectController.h"
#include "CReturnGateController.h"
#include "CRaceBossMsgController.h"
#include "CUserDB.h"
#include "ItemDataLoader.h"
#include "KorLocalTime.h"
#include "MonsterSetInfoData.h"
#include "AggroCaculateData.h"
#include "PatriarchElectProcessor.h"
#include "TimeItem.h"
#include "WheatyExceptionReport.h"
#include "WorldServerUtil.h"
#include "cStaticMember_Player.h"
#include "GlobalObjects.h"

#include "LendItemMng.h"
#include "TimeLimitJadeMng.h"
#include "DfAIMgr.h"
#include "RFEvent_ClassRefine.h"

CMainThread::CMainThread() = default;

bool CMainThread::IsTestServer() const
{
  return m_byWorldType == 1;
}

bool CMainThread::IsReleaseServiceMode() const
{
  return m_bReleaseServiceMode;
}

unsigned int CMainThread::GetMonsterRecordNum() const
{
  return m_tblMonster.GetRecordNum();
}

_DB_QRY_SYN_DATA *CMainThread::PushDQSData(
  unsigned int dwAccountSerial,
  _CLID *pidWorld,
  unsigned __int8 byQryCase,
  char *pQryData,
  int nSize)
{
  unsigned int outIndex[5]{};
  if (!m_listDQSDataEmpty.PopNode_Front(outIndex))
  {
    ServerProgramExit("m_listDQSDataEmpty.PopNode_Front() => failed", 1);
  }

  _DB_QRY_SYN_DATA *data = &m_DBQrySynData[outIndex[0]];
  data->m_dwAccountSerial = dwAccountSerial;
  if (pidWorld)
  {
    memcpy_0(&data->m_idWorld, pidWorld, sizeof(data->m_idWorld));
  }
  data->m_byQryCase = byQryCase;
  if (pQryData)
  {
    memcpy_0(data->m_sData, pQryData, nSize);
  }

  if (m_listDQSData.PushNode_Back(outIndex[0]))
  {
    data->m_bUse = true;
    data->m_bLoad = false;
    return data;
  }

  m_logSystemError.Write("%d : m_listDQSData.PushNode_Back() => failed ", byQryCase);
  return nullptr;
}

char CMainThread::ms_szClientVerCheck[33]{};

void GuildCreateEventInfo::Init()
{
  m_bStartedEvent = false;
  m_EventInfo = {};
  m_ModifyInfo = {};
  m_dwEstConsumeDalant = 0;
  m_dwEmblemDalant = 0;
  m_tmDataFileCheckTime.BeginTimer(0);
  std::memset(&m_ftWrite, 0, sizeof(m_ftWrite));
}

TimeLimitMgr *TimeLimitMgr::Instance()
{
  static TimeLimitMgr s_instance;
  return &s_instance;
}

void TimeLimitMgr::LoadTLINIFile()
{
  char returnedString[48]{};
  char buffer[28]{};

  GetPrivateProfileStringA("Time Limit", "Use", "FALSE", returnedString, sizeof(returnedString), ".\\Initialize\\WorldSystem.ini");
  if (std::strcmp(returnedString, "FALSE") == 0)
  {
    SetTLEnable(0);
  }
  else
  {
    SetTLEnable(1);
  }

  m_wPeriodCnt = static_cast<unsigned __int16>(GetPrivateProfileIntA("Time Limit", "CNT", 2, ".\\Initialize\\WorldSystem.ini"));

  const unsigned int periodCount = m_wPeriodCnt;
  m_pwTime = new unsigned __int16[periodCount];
  m_pdPercent = new long double[periodCount];
  m_pwFatigue = new unsigned __int16[periodCount];

  for (int j = 0; j < m_wPeriodCnt; ++j)
  {
    std::memset(buffer, 0, sizeof(buffer));
    sprintf_s(buffer, sizeof(buffer), "Time_%d", j);
    m_pwTime[j] = static_cast<unsigned __int16>(
      GetPrivateProfileIntA("Time Limit", buffer, 60 * (j + 1), ".\\Initialize\\WorldSystem.ini"));

    std::memset(returnedString, 0, sizeof(returnedString));
    std::memset(buffer, 0, sizeof(buffer));
    sprintf_s(buffer, sizeof(buffer), "Percent_%d", j);
    GetPrivateProfileStringA("Time Limit", buffer, "1.0", returnedString, sizeof(returnedString), ".\\Initialize\\WorldSystem.ini");
    m_pdPercent[j] = std::atof(returnedString);
  }

  for (int k = 0; k < m_wPeriodCnt; ++k)
  {
    m_pwFatigue[k] = static_cast<unsigned __int16>(100 * m_pwTime[k] / m_pwTime[m_wPeriodCnt - 1]);
  }

  m_dwLogoutTerm = GetPrivateProfileIntA("Time Limit", "LogoutTerm", 300, ".\\Initialize\\WorldSystem.ini");
  m_dwNotifyTerm = GetPrivateProfileIntA("Time Limit", "NotifyTerm", 300, ".\\Initialize\\WorldSystem.ini");

  char destination[1024]{};
  char source[60]{};
  strcpy_s(destination, sizeof(destination), "TimeLimitMgr::LoadTLINIFile()");
  for (int m = 0; m < m_wPeriodCnt; ++m)
  {
    sprintf_s(
      source,
      sizeof(source),
      ", Time_%d = %d, Percent_%d = %f",
      m,
      m_pwTime[m],
      m,
      static_cast<double>(m_pdPercent[m]));
    strcat_s(destination, source);
    std::memset(source, 0, sizeof(source));
  }
  g_Main.m_logLoadingError.Write(destination);
}

void TimeLimitMgr::InitializeTLMgr()
{
  int scratch[12]{};
  std::memset(scratch, 0xCC, sizeof(scratch));

  m_dwPlayFDegree = 60000 * m_pwTime[m_wPeriodCnt - 1] / 100;
  m_dwLogoutFDegree = 60000 * m_dwLogoutTerm / 100;
  m_tmLoopTime.BeginTimer(m_dwPlayFDegree);
  for (unsigned int j = 0; j < MAX_PLAYER; ++j)
  {
    std::memset(&m_lstTLStaus[j], 0, sizeof(m_lstTLStaus[j]));
  }
  for (unsigned int k = 0; k < MAX_PLAYER; ++k)
  {
    m_lstTLStaus[k].m_bUpdateLogout = true;
  }
}

void TimeLimitMgr::SetTLEnable(unsigned __int16 wState)
{
  m_wEnable = wState;
}

long double TimeLimitMgr::GetPlayerPenalty(unsigned __int16 wIndex)
{
  if (!m_wEnable)
  {
    return 1.0L;
  }

  if (!m_lstTLStaus[wIndex].m_bUse)
  {
    return 0.0L;
  }

  if (m_lstTLStaus[wIndex].m_bAgeLimit)
  {
    return m_lstTLStaus[wIndex].m_dPercent;
  }

  return 1.0L;
}

unsigned __int8 TimeLimitMgr::GetPlayerStatus(unsigned __int16 wIndex)
{
  if (!m_wEnable)
  {
    return 0;
  }
  if (!m_lstTLStaus[wIndex].m_bUse)
  {
    return 99;
  }
  if (m_lstTLStaus[wIndex].m_bAgeLimit)
  {
    return m_lstTLStaus[wIndex].m_byTL_Status;
  }
  return 0;
}

bool CMainThread::Init()
{
  CRtc::GetIntance()->Reg_Fn();
  g_WheatyExceptionReport.SetLogName("ZoneServer_MainLoop");
  g_WheatyExceptionReport.SetDescription("ZoneServer : Exception Program");

  const DWORD startTick = GetTickCount();
  WriteServerStartHistory("Init >> tickcount: %u", startTick);

  m_bWorldOpen = false;
  m_bWorldService = false;
  m_bCheckOverTickCount = false;

  m_tmServerState.BeginTimer(0x2710);
  m_tmrStateMsgGotoWeb.BeginTimer(0xEA60);
  m_dwCheckAccountOldTick = GetLoopTime();

  const DWORD nowTick = GetTickCount();
  if ((0xFFFFFFFFu - nowTick) < 0x2932E000u)
  {
    MyMessageBox("Start Error", "Must Reboot OS To Service");
    return false;
  }

  CreateDirectoryA("..\\ZoneServerLog\\", nullptr);
  CreateDirectoryA("..\\ZoneServerLog\\Systemlog", nullptr);
  clear_file("..\\ZoneServerLog\\Systemlog", 0xF);
  CreateDirectoryA("..\\ZoneServerLog\\ServiceLog", nullptr);
  clear_file("..\\ZoneServerLog\\ServiceLog", 0xF);
  CreateDirectoryA("..\\ZoneServerLog\\DBLog", nullptr);
  clear_file("..\\ZoneServerLog\\DBLog", 0xF);
  CreateDirectoryA("..\\ZoneServerLog\\CharLog", nullptr);
  clear_file("..\\ZoneServerLog\\CharLog", 0xF);
  CreateDirectoryA("..\\SystemSave", nullptr);
  clear_file("..\\SystemSave", 0xF);
  CreateDirectoryA("..\\ZoneServerLog\\BillingLog", nullptr);
  clear_file("..\\ZoneServerLog\\BillingLog", 0xF);
  CreateDirectoryA("..\\ZoneServerLog\\NetLog", nullptr);
  clear_file("..\\ZoneServerLog\\NetLog", 0xF);
  CreateDirectoryA("..\\ZoneServerLog\\ServerExitLog", nullptr);
  clear_file("..\\ZoneServerLog\\ServerExitLog", 0xF);

  const unsigned int korLocalTime = GetKorLocalTime();
  _CrtSetReportHook(MyCrtDebugReportHook);

  char buffer[132]{};
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\Systemlog\\SystemError%u.log", korLocalTime);
  m_logSystemError.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "%s", "..\\ZoneServerLog\\Systemlog\\LoadingProcess.log");
  m_logLoadingError.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\KillMon%u.log", korLocalTime);
  m_logKillMon.SetWriteLogFile(buffer, 1, 0, 0, 0);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Dungeon%u.log", korLocalTime);
  m_logDungeon.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\ServerState%u.log", korLocalTime);
  m_logServerState.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\DTrade%u.log", korLocalTime);
  m_logDTrade.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Guild%u.log", korLocalTime);
  m_logGuild.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Rename%u.log", korLocalTime);
  m_logRename.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\AutoTrade%u.log", korLocalTime);
  m_logAutoTrade.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Event%u.log", korLocalTime);
  m_logEvent.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Move%u.log", korLocalTime);
  m_logMove.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Save%u.log", korLocalTime);
  m_logSave.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\ReturnGate%u.log", korLocalTime);
  m_logReturnGate.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\Hack%u.log", korLocalTime);
  m_logHack.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\PvP%u.log", korLocalTime);
  m_logPvP.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\MonNum_%u.log", korLocalTime);
  m_logMonNum.SetWriteLogFile(buffer, 1, 0, 1, 1);
  sprintf_s(buffer, sizeof(buffer), "..\\ZoneServerLog\\ServiceLog\\CheckBilling_%u.log", korLocalTime);
  m_logBillCheck.SetWriteLogFile(buffer, 1, 0, 1, 1);

  CAsyncLogger::Instance()->Regist(
    CAsyncLogger::ALT_HACKSHIELD_SYSTEM_LOG,
    "..\\ZoneServerLog\\SystemLog\\HacShiled",
    "HS_System",
    1,
    0x36EE80u);
  CAsyncLogger::Instance()->Regist(
    CAsyncLogger::ALT_APEX_SYSTEM_LOG,
    "..\\ZoneServerLog\\SystemLog\\Apex",
    "Apex_System",
    1,
    0x36EE80u);
  CAsyncLogger::Instance()->Regist(
    CAsyncLogger::ALT_FIREGUARD_DETECT_LOG,
    "..\\ZoneServerLog\\SystemLog\\fireguard",
    "CCRFG_SystemLog",
    1,
    0x36EE80u);
  CAsyncLogger::Instance()->Regist(
    CAsyncLogger::ALT_HONOR_GUILD_LOG,
    "..\\ZoneServerLog\\SystemLog\\HonorGuild",
    "HonorGuild_SysLog",
    1,
    0x5265C00u);
  CAsyncLogger::Instance()->Regist(
    CAsyncLogger::ALT_BUY_CASH_ITEM_LOG,
    "..\\ZoneServerLog\\ServiceLog\\PartiallyPaid",
    "BuyCashItemHistory",
    1,
    0x36EE80u);

  m_logLoadingError.Write("Server Load Start!!");

  if (!LoadINI())
  {
    MyMessageBox("CGameServerDoc Error", "LoadINI()");
    return false;
  }
  if (!CheckDefine())
  {
    MyMessageBox("CGameServerDoc Error", "CheckDefine()");
    return false;
  }
  if (!check_dbsyn_data_size())
  {
    MyMessageBox("CGameServerDoc Error", "check_dbsyn_data_size()");
    return false;
  }
  if (!DataFileInit())
  {
    MyMessageBox("CGameServerDoc Error", "DataFileInit()");
    return false;
  }
  if (!ObjectInit())
  {
    MyMessageBox("CGameServerDoc Error", "ObjectInit()");
    return false;
  }
  m_logLoadingError.Write("Game Data Load Complete!!");
  if (!NetworkInit())
  {
    MyMessageBox("CGameServerDoc Error", "NetworkInit() == false");
    return false;
  }

  m_logLoadingError.Write("Network Init Complete!!");
  m_logLoadingError.Write("Map Load Start!!");

  if (!g_MapOper.Init())
  {
    MyMessageBox("CGameServerDoc Error", "g_MapOper.Init() == false");
    return false;
  }
  if (!g_MonsterEventRespawn.SetEventRespawn())
  {
    MyMessageBox("CGameServerDoc Error", "Read Error Monster Respawn Script");
    return false;
  }

  char errCode[1028]{};
  if (!g_MonsterEventSet.LoadEventSet(errCode))
  {
    MyMessageBox("CGameServerDoc Error", "Read Error Event Set Script, Reason : %s", errCode);
    return false;
  }
  if (!g_MonsterEventSet.LoadEventSetLooting())
  {
    MyMessageBox("CGameServerDoc Error", "Read Error Event Set Looting Script");
    return false;
  }

  m_logLoadingError.Write("Map Load Complete!!");

  const int asyncInit = CAsyncLogger::Instance()->Init();
  if (asyncInit != 0)
  {
    MyMessageBox(
      "CMainThread::Init() : ",
      "CAsyncLogger::Instance()->Init() Ret(%d) Fail!\r\nCheck LoadingProcess.Log!",
      asyncInit);
    m_logLoadingError.Write("CAsyncLogger::Instace()->Init() Ret(%d) Fail!", asyncInit);
    return false;
  }

  m_pTimeLimitMgr = TimeLimitMgr::Instance();
  m_pTimeLimitMgr->LoadTLINIFile();
  m_pTimeLimitMgr->InitializeTLMgr();

  if (!g_DarkHoleQuest.LoadDarkHoleQuest())
  {
    MyMessageBox("DarkHole", "Failed g_DarkHoleQuest.LoadDarkHoleQuest()");
    m_logLoadingError.Write("Failed g_DarkHoleQuest.LoadDarkHoleQuest()");
    return false;
  }

  if (!CGuildBattleController::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CGuildBattleController::Instance()->Init() == false");
    m_logLoadingError.Write("CGuildBattleController::Instance()->Init() == false");
    return false;
  }
  if (!CTotalGuildRankManager::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CTotalGuildRankManager::Instance()->Init() Fail!");
    m_logLoadingError.Write("CTotalGuildRankManager::Instance()->Init() Fail!");
    return false;
  }
  if (!CWeeklyGuildRankManager::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CWeeklyGuildRankManager::Instance()->Init() Fail!");
    m_logLoadingError.Write("CWeeklyGuildRankManager::Instance()->Init() Fail!");
    return false;
  }
  if (!CandidateMgr::Instance()->Initialize(500))
  {
    MyMessageBox("CMainThread::Init() : ", "CandidateMgr::Instance()->Initialize(500) Fail!");
    m_logLoadingError.Write("CandidateMgr::Instance()->Initialize(500) Fail!");
    return false;
  }
  if (!PatriarchElectProcessor::Instance()->Initialize())
  {
    MyMessageBox("CMainThread::Init() : ", "PatriarchElectProcessor::Instance()->Initialize() Fail!");
    m_logLoadingError.Write("PatriarchElectProcessor::Instance()->Initialize() Fail!");
    return false;
  }
  if (!AutominePersonalMgr::instance()->initialize())
  {
    MyMessageBox("ObjectInit() Error", "AutominePersonalMgr::instance()->Initialize() Fail!");
    m_logLoadingError.Write("AutominePersonalMgr::instance()->Initialize() Fail!");
    return false;
  }
  if (!CGuildRoomSystem::GetInstance()->Init())
  {
    MyMessageBox("CMainThread::_GameDataBaseInit()", "CGuildRoomSystem::GetInstance()->Init() Fail!");
    m_logLoadingError.Write("CGuildRoomSystem::GetInstance()->Init() Fail!");
    return false;
  }
  if (!CUnmannedTraderController::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CUnmannedTraderController::Instance()->Init() Fail!");
    m_logLoadingError.Write("CUnmannedTraderController::Instance()->Init() Fail!");
    return false;
  }
  if (!CLogTypeDBTaskManager::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CLogTypeDBTaskManager::Instance()->Init() Fail!");
    m_logLoadingError.Write("CLogTypeDBTaskManager::Instance()->Init() Fail!");
    return false;
  }
  if (!TimeItem::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "TimeItem::Instance()->Init() Fail!");
    m_logLoadingError.Write("TimeItem::Instance()->Init() Fail!");
    return false;
  }
  if (!CTSingleton<CCashDBWorkManager>::Instance()->Initialize())
  {
    MyMessageBox("CMainThread::Init() : ", "CashDbWorker::Instance()->Initialize() Fail!");
    m_logLoadingError.Write("CashDbWorker::Instance()->Initialize() Fail!");
    return false;
  }
  if (!CashItemRemoteStore::Instance()->Initialize())
  {
    MyMessageBox("CMainThread::Init() : ", "CashItemRemoteStore::Instance()->Initialize() Fail!");
    m_logLoadingError.Write("CashItemRemoteStore::Instance()->Initialize() Fail!");
    return false;
  }
  if (!CPostSystemManager::Instace()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CPostSystemManager::Instace()->Init() Fail!");
    m_logLoadingError.Write("CPostSystemManager::Instace()->Init() Fail!");
    return false;
  }
  if (!CPvpUserAndGuildRankingSystem::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CPvpUserAndGuildRankingSystem::Instace()->Init() Fail!");
    m_logLoadingError.Write("CPvpUserAndGuildRankingSystem::Instace()->Init() Fail!");
    return false;
  }
  if (!CMoveMapLimitManager::Instance()->Init())
  {
    MyMessageBox(
      "CMainThread::Init() : ",
      "CMoveMapLimitManager::Instance()->Init() Fail!\r\nCheck LoadingProcess.Log!");
    m_logLoadingError.Write("CPvpUserAndGuildRankingSystem::Instace()->Init() Fail!");
    return false;
  }
  if (!CRaceBuffManager::Instance()->Init())
  {
    MyMessageBox(
      "CMainThread::Init() : ",
      "CRaceBuffManager::Instance()->Init() Fail!\r\nCheck LoadingProcess.Log!");
    m_logLoadingError.Write("CRaceBuffManager::Instace()->Init() Fail!");
    return false;
  }
  if (!CHonorGuild::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CHonorGuild::Instance()->Init() Fail!");
    m_logLoadingError.Write("CHonorGuild::Instance()->Init() Fail!");
    return false;
  }
  if (!CExchangeEvent::Instance()->Initialzie())
  {
    MyMessageBox("CMainThread::Init() : ", "CExchangeEvent::Instance()->Initialzie() Fail!");
    m_logLoadingError.Write("CExchangeEvent::Instance()->Initialzie() Fail!");
    return false;
  }
  if (!CPcBangFavor::Instance()->Initialzie())
  {
    MyMessageBox("CMainThread::Init() : ", "CPcBangFavor::Instance()->Initialzie() Fail!");
    m_logLoadingError.Write("CPcBangFavor::Instance()->Initialzie() Fail!");
    return false;
  }
  if (!CActionPointSystemMgr::Instance()->Initialize())
  {
    MyMessageBox("CMainThread::Init() : ", "CActionPointSystemMgr::Instance()->Initialize() Fail!");
    m_logLoadingError.Write("CActionPointSystemMgr::Instance()->Initialize() Fail");
    return false;
  }

  m_GuildCreateEventInfo.Init();
  m_GameMsg.Init(100);
  m_tmrCheckAvator.BeginTimer(0x3E8);
  m_tmrCheckLoop.BeginTimer(0x3E8);
  m_tmrAccountPing.BeginTimer(0x3E8);
  m_tmrCheckRadarDelay.BeginTimer(0x2710);

  if (!CLuaScriptMgr::Instance()->InitSDM())
  {
    MyMessageBox("CMainThread::Init() : ", "CLuaScriptMgr::Instance()->InitSDM() Fail!");
    m_logLoadingError.Write("CLuaScriptMgr::Instance()->InitSDM()");
    return false;
  }

  if (!CTSingleton<CCryptor>::Instance()->Init(".\\Initialize\\WorldSystem.bin", 0))
  {
    MyMessageBox("CMainThread::Init() : ", "CCryptor::Instance()->Init() Fail");
    m_logLoadingError.Write("CCryptor::Instance()->Init()");
    return false;
  }

  if (!CBossMonsterScheduleSystem::Instance()->Init(&g_MapOper))
  {
    MyMessageBox("CMainThread::Init() : ", "CBossMonsterScheduleSystem::Instance()->Init(&g_MapOper) Fail!");
    m_logLoadingError.Write("CBossMonsterScheduleSystem::Instance()->Init(&g_MapOper) Fail");
    return false;
  }

  m_kEtcNotifyInfo.Init();

  if (!cStaticMember_Player::Instance()->Initialize())
  {
    MyMessageBox("CMainThread::Init() : ", "cStaticMember_Player::Instance()->Initialize() Fail!");
    m_logLoadingError.Write("cStaticMember_Player::Instance()->Initialize() Fail!");
    return false;
  }

  m_bRuleThread = true;
  _beginthread(CMainThread::RuleThread, 0, this);
  m_bDQSThread = true;
  _beginthread(CMainThread::DQSThread, 0, this);

  m_logLoadingError.Write("Thread Setting Complete!!");

  m_MgrConnNum.Init();
  m_HisMainFPS.Init();
  m_HisSendFPS.Init();
  m_HisDataFPS.Init();

  if (g_pFrame == nullptr)
  {
    g_pFrame = AfxGetMainWnd();
  }
  if (g_pFrame != nullptr)
  {
    g_pFrame->SendMessage(0x000C, 0, 0);
  }

  MakeSystemTower();
  m_logLoadingError.Write("System Tower Make Complete!!");

  m_byWebAgentServerNetInx = 0;
  m_bConnectedWebAgentServer = false;
  m_byControllServerNetInx = 0;
  m_bConnectedControllServer = false;

  if (!CTSingleton<CBillingManager>::Instance()->Init())
  {
    MyMessageBox("CMainThread::Init() : ", "CBillingManager::Init() Fail!");
    m_logLoadingError.Write("CBillingManager::Init() Fail!");
    return false;
  }

  m_dwCheatSetPlayTime = 600;
  m_dwCheatSetScanerCnt = 0;
  m_dwCheatSetLevel = 50;
  m_dwServerResetToken = 1990011;

  if (!CGoldenBoxItemMgr::Instance()->Initialize())
  {
    MyMessageBox("CMainThread::Init() : ", "CGoldenBoxItemMgr::Instance()->Initialize() Fail!");
    m_logLoadingError.Write("CGoldenBoxItemMgr::Instance()->Initialize() Fail!");
    return false;
  }

  return true;
}

bool CMainThread::LoadINI()
{
  const int worldSystemRet = LoadWorldSystemINI();
  if (worldSystemRet != 0)
  {
    MyMessageBox("CMainThread::LoadINI()", "LoadWorldSystemINI() iRet(%d) Fail!", worldSystemRet);
    return false;
  }

  const int worldInfoRet = LoadWorldInfoINI();
  if (worldInfoRet != 0)
  {
    MyMessageBox("CMainThread::LoadINI()", "LoadWorldInfoINI() iRet(%d) Fail!", worldInfoRet);
    return false;
  }

  LoadItemConsumeINI();
  return true;
}

int CMainThread::LoadWorldSystemINI()
{
  char returnedString[128]{};
  GetPrivateProfileStringA(
    "System",
    "AccountAddress",
    "X",
    returnedString,
    static_cast<DWORD>(sizeof(returnedString)),
    ".\\Initialize\\WorldSystem.ini");
  if (strcmp(returnedString, "X") == 0)
  {
    return -1;
  }
  IN_ADDR addr{};
  if (InetPtonA(AF_INET, returnedString, &addr) != 1)
  {
    return -1;
  }
  m_dwAccountIP = addr.s_addr;

  GetPrivateProfileStringA(
    "VersionCheck",
    "Ver_CheckKey",
    "X",
    ms_szClientVerCheck,
    static_cast<DWORD>(sizeof(ms_szClientVerCheck)),
    ".\\Initialize\\WorldSystem.ini");
  return 0;
}

int CMainThread::LoadWorldInfoINI()
{
  GetPrivateProfileStringA(
    "System",
    "WorldName",
    "X",
    m_szWorldName,
    static_cast<DWORD>(sizeof(m_szWorldName)),
    "..\\WorldInfo\\WorldInfo.ini");
  if (strcmp(m_szWorldName, "X") == 0)
  {
    return -1;
  }
  strncpy_s(m_wszWorldName, sizeof(m_wszWorldName), m_szWorldName, _TRUNCATE);

  m_bFreeServer = GetPrivateProfileIntA("System", "FreeServer", 0, "..\\WorldInfo\\WorldInfo.ini");
  m_byWorldType = static_cast<unsigned __int8>(
    GetPrivateProfileIntA("System", "ServerType", 2, "..\\WorldInfo\\WorldInfo.ini"));

  char releaseType[9]{};
  GetPrivateProfileStringA(
    "ServerMode",
    "ReleaseType",
    "X",
    releaseType,
    static_cast<DWORD>(sizeof(releaseType)),
    "..\\WorldInfo\\WorldInfo.ini");
  if (_stricmp(releaseType, "Internal") == 0)
  {
    m_bReleaseServiceMode = false;
  }
  else if (_stricmp(releaseType, "Release") == 0)
  {
    m_bReleaseServiceMode = true;
  }
  else
  {
    MyMessageBox(
      "CMainThread::LoadWorldSystemINI()",
      "WorldInfo.ini\r\n[ServerMode]\r\nReleaseType = %s Invalid!!",
      releaseType);
    return -2;
  }

  char executeService[6]{};
  GetPrivateProfileStringA(
    "ServerMode",
    "ExcuteService",
    "X",
    executeService,
    static_cast<DWORD>(sizeof(executeService)),
    "..\\WorldInfo\\WorldInfo.ini");
  if (_stricmp(executeService, "true") == 0)
  {
    m_bExcuteService = true;
  }
  else if (_stricmp(executeService, "false") == 0)
  {
    m_bExcuteService = false;
  }
  else
  {
    MyMessageBox(
      "CMainThread::LoadWorldSystemINI()",
      "WorldInfo.ini\r\n[ServerMode]\r\nExcuteService = %s Invalid!!",
      executeService);
    return -3;
  }

  char nationCode[3]{};
  GetPrivateProfileStringA(
    "System",
    "NationCode",
    "X",
    nationCode,
    static_cast<DWORD>(sizeof(nationCode)),
    "..\\WorldInfo\\WorldInfo.ini");

  CNationCodeStrTable table;
  if (!table.Init())
  {
    return -4;
  }

  const int type = table.GetCode(nationCode);
  if (type == -1)
  {
    MyMessageBox(
      "CMainThread::LoadWorldSystemINI()",
      "WorldInfo.ini\r\n[System]\r\nNationCode = %s Invalid!!",
      nationCode);
    return -5;
  }

  const char *codeStr = table.GetStr(type);
  strncpy_s(nationCode, sizeof(nationCode), codeStr, _TRUNCATE);
  const int initRet = CNationSettingManager::Instance()->Init(type, nationCode, m_bReleaseServiceMode);
  if (initRet != 0)
  {
    const char *serviceMode = m_bReleaseServiceMode ? "true" : "false";
    MyMessageBox(
      "CMainThread::LoadWorldSystemINI()",
      "CNationSettingManager::Instance()->Init( iNationCode(%d), szNationCodeStr(%s), bServiceMode(%s) ) :  iRet(%d) Fail!",
      type,
      nationCode,
      serviceMode,
      initRet);
    return -6;
  }

  return 0;
}

void CMainThread::LoadItemConsumeINI()
{
  char returnedString[10]{};
  GetPrivateProfileStringA(
    "AWAY_PARTY",
    "ItemConsume",
    "TRUE",
    returnedString,
    static_cast<DWORD>(sizeof(returnedString)),
    ".\\Initialize\\ItemConsume.ini");
  m_bAwayPartyConsumeItem = strcmp(returnedString, "TRUE") == 0;

  GetPrivateProfileStringA(
    "AWAY_PARTY",
    "ItemCode",
    "ircht01",
    m_strAwayPartyItemCode,
    static_cast<DWORD>(sizeof(m_strAwayPartyItemCode)),
    ".\\Initialize\\ItemConsume.ini");

  char moneyConsume[10]{};
  GetPrivateProfileStringA(
    "AWAY_PARTY",
    "MoneyConsume",
    "FALSE",
    moneyConsume,
    static_cast<DWORD>(sizeof(moneyConsume)),
    ".\\Initialize\\ItemConsume.ini");
  m_bAwayPartyConsumeMoney = strcmp(moneyConsume, "TRUE") == 0;
  m_dwAwayPartyMoney = static_cast<unsigned int>(
    GetPrivateProfileIntA("AWAY_PARTY", "Money", 0, ".\\Initialize\\ItemConsume.ini"));

  GetPrivateProfileStringA(
    "AllRaceChat",
    "ItemCode",
    "X",
    m_strAllRaceChatItemCode,
    static_cast<DWORD>(sizeof(m_strAllRaceChatItemCode)),
    ".\\Initialize\\ItemConsume.ini");

  char itemConsume[10]{};
  GetPrivateProfileStringA(
    "AllRaceChat",
    "ItemConsume",
    "TRUE",
    itemConsume,
    static_cast<DWORD>(sizeof(itemConsume)),
    ".\\Initialize\\ItemConsume.ini");
  m_bAllRaceChatItemConsume = strcmp(itemConsume, "TRUE") == 0;

  GetPrivateProfileStringA(
    "AllRaceChat",
    "MoneyConsume",
    "FALSE",
    itemConsume,
    static_cast<DWORD>(sizeof(itemConsume)),
    ".\\Initialize\\ItemConsume.ini");
  m_bAllRaceChatMoneyConsume = strcmp(itemConsume, "TRUE") == 0;
  m_dwAllRaceChatMoney = static_cast<unsigned int>(
    GetPrivateProfileIntA("AllRaceChat", "Money", 0, ".\\Initialize\\ItemConsume.ini"));
}

bool CMainThread::CheckDefine()
{
  return true; //this is not stub, original code is already like this
}

bool CMainThread::check_dbsyn_data_size()
{
  return true; //this is not stub, original code is already like this
}

bool CMainThread::DataFileInit()
{
  char szErrCode[152]{};
  if (!WriteTableData(37, m_tblItemData, true, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }

  const char *effectFiles[] =
  {
    ".\\script\\skill.dat",
    ".\\script\\force.dat",
    ".\\script\\ClassSkill.dat",
    ".\\script\\BulletItemEffect.dat",
  };
  const unsigned int effectSizes[] = {1168, 1088, 1168, 1168};
  for (int i = 0; i < 4; ++i)
  {
    if (!m_tblEffectData[i].ReadRecord(effectFiles[i], static_cast<int>(effectSizes[i]), szErrCode))
    {
      MyMessageBox("DatafileInit", szErrCode);
      return false;
    }
  }

  if (!g_PotionMgr.DatafileInit())
  {
    return false;
  }

  CPlayer::ms_pXmas_Snow_Effect =
    reinterpret_cast<_skill_fld *>(m_tblEffectData[3].GetRecord("16"));
  CPlayer::ms_pXmas_Snow_Bullet_Effect =
    reinterpret_cast<_skill_fld *>(m_tblEffectData[3].GetRecord("15"));
  if (!CPlayer::ms_pXmas_Snow_Effect || !CPlayer::ms_pXmas_Snow_Bullet_Effect)
  {
    MyMessageBox(
      "MILKSIK_X_MAS_2006 Error",
      "m_tblEffectData[effect_code_throw].GetRecord( '%s' ) == NULL",
      "16");
    return false;
  }

  if (!m_tblClass.ReadRecord(".\\Script\\Class.dat", 0x8D0, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblGrade.ReadRecord(".\\Script\\Grade.dat", 0x4C, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblPlayer.ReadRecord(".\\Script\\PlayerCharacter.dat", 0xA8, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblMonster.ReadRecord(".\\Script\\MonsterCharacter.dat", 0x9B4, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }

  CRecordData mobMessage;
  if (!mobMessage.ReadRecord(".\\Script\\MobMessage_str.dat", 0x44C48, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  const unsigned int mobCount = mobMessage.GetRecordNum();
  if (mobCount > 0)
  {
    m_MobMessage = new _mob_message[mobCount]{};
    for (unsigned int n = 0; n < mobCount; ++n)
    {
      _mobmsg_fld *record = reinterpret_cast<_mobmsg_fld *>(mobMessage.GetRecord(static_cast<int>(n)));
      if (record == nullptr)
      {
        MyMessageBox("DatafileInit", szErrCode);
        return false;
      }
      m_MobMessage[n].wIndex = static_cast<unsigned __int16>(atoi(record->m_strCode));
      m_MobMessage[n].byUsingNum = static_cast<unsigned __int8>(record->m_nUsingNum);
    }
  }

  if (!m_tblNPC.ReadRecord(".\\Script\\NPCharacter.dat", 0x1E8, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblAnimus.ReadRecord(".\\Script\\AnimusItem.dat", 0x188, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblExp.ReadRecord(".\\Script\\Exp.dat", 0x104, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblItemLoot.ReadRecord(".\\Script\\ItemLooting.dat", m_tblItemData, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblOreCutting.ReadRecord(
        ".\\Script\\OreCutting.dat",
        &m_tblItemData[17],
        &m_tblItemData[18],
        szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblItemMakeData.ReadRecord(".\\Script\\ItemMakeData.dat", 0x22C, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblItemCombineData.ReadRecord(".\\Script\\ItemCombine.dat", 0xD0, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblItemExchangeData.ReadRecord(".\\Script\\BoxItemOut.dat", 0x414, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblItemUpgrade.ReadRecord(".\\Script\\ItemUpgrade.dat", &m_tblItemData[18], szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }

  const char *unitPartFiles[] =
  {
    ".\\script\\UnitHead.dat",
    ".\\script\\UnitUpper.dat",
    ".\\script\\UnitLower.dat",
    ".\\script\\UnitArms.dat",
    ".\\script\\UnitShoulder.dat",
    ".\\script\\UnitBack.dat",
  };
  for (int i = 0; i < 6; ++i)
  {
    if (!m_tblUnitPart[i].ReadRecord(unitPartFiles[i], 0x204, szErrCode))
    {
      MyMessageBox("DatafileInit", szErrCode);
      return false;
    }
  }

  if (!m_tblUnitBullet.ReadRecord(".\\script\\UnitBullet.dat", 0x164, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblUnitFrame.ReadRecord(".\\script\\UnitFrame.dat", 0x288, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_tblEditData.ReadRecord(".\\script\\EditData.dat", 0x83C, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_MonsterBaseSPData.ReadRecord(".\\script\\MonsterCharacterAI.dat", 0xB0, szErrCode))
  {
    MyMessageBox("DatafileInit", szErrCode);
    return false;
  }
  if (!m_MonsterSPGroupTable.Create(
        &m_tblMonster,
        &m_MonsterBaseSPData,
        m_tblEffectData,
        &m_tblEffectData[1],
        &m_tblEffectData[2]))
  {
    MyMessageBox("DatafileInit", " m_MonsterSPGroupTable.Create()... fail");
    return false;
  }
  if (!CQuestMgr::LoadQuestData())
  {
    return false;
  }
  if (!ItemCombineMgr::LoadData())
  {
    return false;
  }
  if (!CPcBangFavor::Instance()->LoadPcBangData())
  {
    return false;
  }
  if (!CSUItemSystem::Instance()->SUItemSystem_Init())
  {
    return false;
  }
  if (!g_AggroCaculateData.Load(".\\Initialize\\AP.ini"))
  {
    MyMessageBox("CMainThread::DataFileInit()", "DatafileInit->AggroSystem %s Loading Fail", ".\\Initialize\\AP.ini");
    return false;
  }
  if (!g_MonsterSetInfoData.Load(".\\Initialize\\MonsterSet.ini"))
  {
    MyMessageBox(
      "CMainThread::DataFileInit()",
      "DatafileInit->g_MonsterSetInfoData %s Loading Fail",
      ".\\Initialize\\MonsterSet.ini");
    return false;
  }
  if (!SetGlobalDataName())
  {
    return false;
  }
  if (!check_loaded_data())
  {
    return false;
  }
  gm_MainThreadControl();
  return true;
}

bool CMainThread::SetGlobalDataName()
{
  char errMsg[160]{};

  const char *itemNameFiles[37] =
  {
    ".\\script\\UpperItem_str.dat", //_DfnEquipItem_fld
    ".\\script\\LowerItem_str.dat",//_DfnEquipItem_fld
    ".\\script\\GauntletItem_str.dat",//_DfnEquipItem_fld
    ".\\script\\ShoeItem_str.dat",//_DfnEquipItem_fld
    ".\\script\\HelmetItem_str.dat",//_DfnEquipItem_fld
    ".\\script\\ShieldItem_str.dat",//_DfnEquipItem_fld
    ".\\script\\WeaponItem_str.dat",//_WeaponItem_fld
    ".\\script\\CloakItem_str.dat",//_CloakItem_fld
    ".\\script\\RingItem_str.dat",//_RingItem_fld
    ".\\script\\AmuletItem_str.dat",//_AmuletItem_fld
    ".\\script\\BulletItem_str.dat",//_BulletItem_fld
    ".\\script\\MaketoolItem_str.dat",//_MakeToolItem_fld
    ".\\script\\BagItem_str.dat",//_BagItem_fld
    ".\\script\\PotionItem_str.dat",//_PotionItem_fld
    ".\\script\\FaceItem_str.dat",//_FaceItem_fld
    ".\\script\\ForceItem_str.dat",//_ForceItem_fld
    ".\\script\\BatteryItem_str.dat",//_BatteryItem_fld
    ".\\script\\OreItem_str.dat",//_OreItem_fld
    ".\\script\\ResourceItem_str.dat",//_ResourceItem_fld
    ".\\script\\UnitKeyItem_str.dat",//_ForceItem_fld
    ".\\script\\BootyItem_str.dat",//_BootyItem_fld
    ".\\script\\MapItem_str.dat",//_MapItem_fld
    ".\\script\\TOWNItem_str.dat",//_TOWNItem_fld
    ".\\script\\BattleDungeonItem_str.dat",//_BattleDungeonItem_fld
    ".\\script\\AnimusItem_str.dat",//_AnimusItem_fld
    ".\\script\\GuardTowerItem_str.dat",//_GuardTowerItem_fld
    ".\\script\\TrapItem_str.dat",//_TrapItem_fld
    ".\\script\\SiegeKitItem_str.dat",//_SiegeKitItem_fld
    ".\\script\\TicketItem_str.dat",//_TicketItem_fld
    ".\\script\\EventItem_str.dat",//_EventItem_fld
    ".\\script\\RecoveryItem_str.dat",//_RecoveryItem_fld
    ".\\script\\BoxItem_str.dat",//_BoxItem_fld
    ".\\script\\FIRECRACKER_str.dat",//_FIRECRACKER_fld
    ".\\script\\UNmannedminer_str.dat",//_UNmannedminer_fld
    ".\\script\\RadarItem_str.dat",//_RadarItem_fld
    ".\\script\\NPCLinkItem_str.dat",//_NPCLink_fld
    ".\\script\\CouponItem_str.dat",//_CouponItem_fld
  };

  CRecordData itemNameTables[37];
  for (int j = 0; j < 37; ++j)
  {
    if (!itemNameTables[j].ReadRecord(itemNameFiles[j], 0x304, errMsg))
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Item Name Data Load Error : ItemTableCode(%d), ErrorMsg(%s)",
        j,
        errMsg);
      return false;
    }

    CRecordData *itemTable = &m_tblItemData[j];
    if (!itemTable || !itemTable->IsTableOpen())
    {
      MyMessageBox("SetGlobalDataName()", "ItemData Not Loaded : ItemTableCode(%d)", j);
      return false;
    }

    const int recordNum = static_cast<int>(itemTable->GetRecordNum());
    for (int n = 0; n < recordNum; ++n)
    {
      _base_fld *record = itemTable->GetRecord(n);
      if (!record)
      {
        MyMessageBox("SetGlobalDataName()", "Get Item Record Error : ItemTableCode(%d), ItemIndex(%d)", j, n);
        return false;
      }

      char *destination = nullptr;
      switch (j)
      {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
          destination = reinterpret_cast<_DfnEquipItem_fld *>(record)->m_strName;
          break;
        case 6:
          destination = reinterpret_cast<_WeaponItem_fld *>(record)->m_strName;
          break;
        case 7:
          destination = reinterpret_cast<_CloakItem_fld *>(record)->m_strName;
          break;
        case 8:
          destination = reinterpret_cast<_RingItem_fld *>(record)->m_strName;
          break;
        case 9:
          destination = reinterpret_cast<_AmuletItem_fld *>(record)->m_strName;
          break;
        case 10:
          destination = reinterpret_cast<_BulletItem_fld *>(record)->m_strName;
          break;
        case 11:
          destination = reinterpret_cast<_MakeToolItem_fld *>(record)->m_strName;
          break;
        case 12:
          destination = reinterpret_cast<_BagItem_fld *>(record)->m_strName;
          break;
        case 13:
          destination = reinterpret_cast<_PotionItem_fld *>(record)->m_strName;
          break;
        case 14:
          destination = reinterpret_cast<_FaceItem_fld *>(record)->m_strName;
          break;
        case 15:
          destination = reinterpret_cast<_ForceItem_fld *>(record)->m_strName;
          break;
        case 16:
          destination = reinterpret_cast<_BatteryItem_fld *>(record)->m_strName;
          break;
        case 17:
          destination = reinterpret_cast<_OreItem_fld *>(record)->m_strName;
          break;
        case 18:
          destination = reinterpret_cast<_ResourceItem_fld *>(record)->m_strName;
          break;
        case 19:
          destination = reinterpret_cast<_UnitKeyItem_fld *>(record)->m_strName;
          break;
        case 20:
          destination = reinterpret_cast<_BootyItem_fld *>(record)->m_strName;
          break;
        case 21:
          destination = reinterpret_cast<_MapItem_fld *>(record)->m_strName;
          break;
        case 22:
          destination = reinterpret_cast<_TOWNItem_fld *>(record)->m_strName;
          break;
        case 23:
          destination = reinterpret_cast<_BattleDungeonItem_fld *>(record)->m_strName;
          break;
        case 24:
          destination = reinterpret_cast<_AnimusItem_fld *>(record)->m_strName;
          break;
        case 25:
          destination = reinterpret_cast<_GuardTowerItem_fld *>(record)->m_strName;
          break;
        case 26:
          destination = reinterpret_cast<_TrapItem_fld *>(record)->m_strName;
          break;
        case 27:
          destination = reinterpret_cast<_SiegeKitItem_fld *>(record)->m_strName;
          break;
        case 28:
          destination = reinterpret_cast<_TicketItem_fld *>(record)->m_strName;
          break;
        case 29:
          destination = reinterpret_cast<_EventItem_fld *>(record)->m_strName;
          break;
        case 30:
          destination = reinterpret_cast<_RecoveryItem_fld *>(record)->m_strName;
          break;
        case 31:
          destination = reinterpret_cast<_BoxItem_fld *>(record)->m_strName;
          break;
        case 32:
          destination = reinterpret_cast<_FIRECRACKER_fld *>(record)->m_strName;
          break;
        case 33:
          destination = reinterpret_cast<_UNmannedminer_fld *>(record)->m_strName;
          break;
        case 34:
          destination = reinterpret_cast<_RadarItem_fld *>(record)->m_strName;
          break;
        case 35:
          destination = reinterpret_cast<_NPCLink_fld *>(record)->m_strName;
          break;
        case 36:
          destination = reinterpret_cast<_CouponItem_fld *>(record)->m_strName;
          break;
        default:
          destination = nullptr;
          break;
      }

      if (destination == nullptr)
      {
        MyMessageBox("SetGlobalDataName()", "Unknown Item Table Code : ItemTableCode(%d)", j);
        return false;
      }

      _NameTxt_fld *nameRecord =
        reinterpret_cast<_NameTxt_fld *>(itemNameTables[j].GetRecord(record->m_strCode));
      if (!nameRecord)
      {
        MyMessageBox(
          "SetGlobalDataName()",
          "Get Item Name Error : Not Exist Item Name, ItemTableCode(%d), ItemCode(%s)",
          j,
          record->m_strCode);
        return false;
      }

      const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
      strcpy_s(destination, 0x40, name);
    }
  }

  const char *effectNameFiles[4] =
  {
    ".\\script\\skill_str.dat",
    ".\\script\\force_str.dat",
    ".\\script\\ClassSkill_str.dat",
    ".\\script\\BulletItemEffect_str.dat",
  };

  CRecordData effectNameTables[4];
  for (int j = 0; j < 4; ++j)
  {
    if (!effectNameTables[j].ReadRecord(effectNameFiles[j], 0x304, errMsg))
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Effect Name Data Load Error : ItemTableCode(%d), ErrorMsg(%s)",
        j,
        errMsg);
      return false;
    }

    CRecordData *effectTable = &m_tblEffectData[j];
    if (!effectTable || !effectTable->IsTableOpen())
    {
      MyMessageBox("SetGlobalDataName()", "EffectData Not Loaded : EffectNum(%d)", j);
      return false;
    }

    const int recordNum = static_cast<int>(effectTable->GetRecordNum());
    for (int n = 0; n < recordNum; ++n)
    {
      _base_fld *baseRecord = effectTable->GetRecord(n);
      if (!baseRecord)
      {
        MyMessageBox("SetGlobalDataName()", "Get Effect Record Error : EffectNum(%d), EffectIndex(%d)", j, n);
        return false;
      }

      _skill_fld *record = reinterpret_cast<_skill_fld *>(baseRecord);
      _NameTxt_fld *nameRecord =
        reinterpret_cast<_NameTxt_fld *>(effectNameTables[j].GetRecord(record->m_strCode));
      if (!nameRecord)
      {
        MyMessageBox(
          "SetGlobalDataName()",
          "Get Effect Name Error : Not Exist Effect Name, EffectNum(%d), EffectCode(%s)",
          j,
          record->m_strCode);
        return false;
      }

      const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
      strcpy_s(record->m_strKorName, 0x40, name);
    }
  }

  if (!g_PotionMgr.SetPotionDataName())
  {
    MyMessageBox("SetGlobalDataName()", "g_PotionMgr.SetPotionDataName() Error!");
    return false;
  }

  if (!m_tblClass.IsTableOpen())
  {
    MyMessageBox("SetGlobalDataName()", "Class Data Not Loaded");
    return false;
  }

  CRecordData classNames;
  if (!classNames.ReadRecord(".\\Script\\Class_str.dat", 0x304, errMsg))
  {
    MyMessageBox("SetGlobalDataName()", "Class Name Data Load Error");
    return false;
  }

  int recordNum = static_cast<int>(m_tblClass.GetRecordNum());
  for (int j = 0; j < recordNum; ++j)
  {
    _class_fld *record = reinterpret_cast<_class_fld *>(m_tblClass.GetRecord(j));
    if (!record)
    {
      MyMessageBox("SetGlobalDataName()", "Get Class Data Error, ClassIndex(%d)", j);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(classNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Get Class Name Error : Not Exist Class Name, ClassCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strKorName, 0x40, name);
  }

  if (!m_tblPlayer.IsTableOpen())
  {
    MyMessageBox("SetGlobalDataName()", "Player Character Data Not Loaded");
    return false;
  }

  CRecordData playerNames;
  if (!playerNames.ReadRecord(".\\Script\\PlayerCharacter_str.dat", 0x304, errMsg))
  {
    MyMessageBox("SetGlobalDataName()", "Player Character Name Data Load Error");
    return false;
  }

  recordNum = static_cast<int>(m_tblPlayer.GetRecordNum());
  for (int j = 0; j < recordNum; ++j)
  {
    _player_fld *record = reinterpret_cast<_player_fld *>(m_tblPlayer.GetRecord(j));
    if (!record)
    {
      MyMessageBox("SetGlobalDataName()", "Get Player Character Data Error, PlayerCharacterIndex(%d)", j);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(playerNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Get Player Character Name Error : Not Exist Player Character Name, PlayerCharacterCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strName, 0x40, name);
  }

  if (!m_tblMonster.IsTableOpen())
  {
    MyMessageBox("SetGlobalDataName()", "Monster Character Data Not Loaded");
    return false;
  }

  CRecordData monsterNames;
  if (!monsterNames.ReadRecord(".\\Script\\MonsterCharacter_str.dat", 0x304, errMsg))
  {
    MyMessageBox("SetGlobalDataName()", "Monster Character Name Data Load Error");
    return false;
  }

  recordNum = static_cast<int>(m_tblMonster.GetRecordNum());
  for (int j = 0; j < recordNum; ++j)
  {
    _monster_fld *record = reinterpret_cast<_monster_fld *>(m_tblMonster.GetRecord(j));
    if (!record)
    {
      MyMessageBox("SetGlobalDataName()", "Get Monster Character Data Error, MonsterCharacterIndex(%d)", j);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(monsterNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Get Monster Character Name Error : Not Exist Monster Character Name, MonsterCharacterCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strName, 0x40, name);
  }

  if (!m_tblNPC.IsTableOpen())
  {
    MyMessageBox("SetGlobalDataName()", "NPC Data Not Loaded");
    return false;
  }

  CRecordData npcNames;
  if (!npcNames.ReadRecord(".\\Script\\NPCharacter_str.dat", 0x304, errMsg))
  {
    MyMessageBox("SetGlobalDataName()", "NPC Name Data Load Error");
    return false;
  }

  recordNum = static_cast<int>(m_tblNPC.GetRecordNum());
  for (int j = 0; j < recordNum; ++j)
  {
    _npc_fld *record = reinterpret_cast<_npc_fld *>(m_tblNPC.GetRecord(j));
    if (!record)
    {
      MyMessageBox("SetGlobalDataName()", "Get NPC Data Error, NPCIndex(%d)", j);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(npcNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Get NPC Name Error : Not Exist NPC Name, NPCCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strName, 0x40, name);
  }

  if (!m_tblAnimus.IsTableOpen())
  {
    MyMessageBox("SetGlobalDataName()", "Animus Item Data Not Loaded");
    return false;
  }

  CRecordData animusNames;
  if (!animusNames.ReadRecord(".\\Script\\AnimusItem_str.dat", 0x304, errMsg))
  {
    MyMessageBox("SetGlobalDataName()", "Animus Item Name Data Load Error");
    return false;
  }

  recordNum = static_cast<int>(m_tblAnimus.GetRecordNum());
  for (int j = 0; j < recordNum; ++j)
  {
    _AnimusItem_fld *record = reinterpret_cast<_AnimusItem_fld *>(m_tblAnimus.GetRecord(j));
    if (!record)
    {
      MyMessageBox("SetGlobalDataName()", "Get Animus Item Data Error, AnimusItemIndex(%d)", j);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(animusNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Get Animus Item Name Error : Not Exist Animus Item Name, AnimusItemCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strName, 0x40, name);
  }

  const char *unitPartFiles[6] =
  {
    ".\\script\\UnitHead_str.dat",
    ".\\script\\UnitUpper_str.dat",
    ".\\script\\UnitLower_str.dat",
    ".\\script\\UnitArms_str.dat",
    ".\\script\\UnitShoulder_str.dat",
    ".\\script\\UnitBack_str.dat",
  };

  CRecordData unitPartNames[6];
  for (int j = 0; j < 6; ++j)
  {
    if (!unitPartNames[j].ReadRecord(unitPartFiles[j], 0x304, errMsg))
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Unit Part Name Data Load Error : UnitPartNum(%d), ErrorMsg(%s)",
        j,
        errMsg);
      return false;
    }

    CRecordData *unitPartTable = &m_tblUnitPart[j];
    if (!unitPartTable || !unitPartTable->IsTableOpen())
    {
      MyMessageBox("SetGlobalDataName()", "Unit Part Data Not Loaded : UnitPartNum(%d)", j);
      return false;
    }

    const int unitRecordNum = static_cast<int>(unitPartTable->GetRecordNum());
    for (int n = 0; n < unitRecordNum; ++n)
    {
      _UnitPart_fld *record = reinterpret_cast<_UnitPart_fld *>(unitPartTable->GetRecord(n));
      if (!record)
      {
        MyMessageBox(
          "SetGlobalDataName()",
          "Get Unit Part Data Error : UnitPartNum(%d), UnitPartIndex(%d)",
          j,
          n);
        return false;
      }

      _NameTxt_fld *nameRecord =
        reinterpret_cast<_NameTxt_fld *>(unitPartNames[j].GetRecord(record->m_strCode));
      if (!nameRecord)
      {
        MyMessageBox(
          "SetGlobalDataName()",
          "Get Unit Part Name Error : Not Exist Unit Part Name, UnitPartCode(%s)",
          record->m_strCode);
        return false;
      }

      const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
      strcpy_s(record->m_strName, 0x40, name);
    }
  }

  if (!m_tblUnitBullet.IsTableOpen())
  {
    MyMessageBox("SetGlobalDataName()", "Unit Bullet Data Not Loaded");
    return false;
  }

  CRecordData unitBulletNames;
  if (!unitBulletNames.ReadRecord(".\\Script\\UnitBullet_str.dat", 0x304, errMsg))
  {
    MyMessageBox("SetGlobalDataName()", "Unit Bullet Name Data Load Error");
    return false;
  }

  recordNum = static_cast<int>(m_tblUnitBullet.GetRecordNum());
  for (int j = 0; j < recordNum; ++j)
  {
    _UnitBullet_fld *record = reinterpret_cast<_UnitBullet_fld *>(m_tblUnitBullet.GetRecord(j));
    if (!record)
    {
      MyMessageBox("SetGlobalDataName()", "Get Unit Bullet Data Error, UnitBulletIndex(%d)", j);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(unitBulletNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "SetGlobalDataName()",
        "Get Unit Bullet Name Error : Not Exist Unit Bullet Name, UnitBulletCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strName, 0x40, name);
  }

  return true;
}

bool CMainThread::check_loaded_data()
{
  bool dataError = false;

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblItemData[18].GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _ResourceItem_fld *record = reinterpret_cast<_ResourceItem_fld *>(m_tblItemData[18].GetRecord(n));
    if (record->m_nAncStdPrice != record->m_nExStdPrice || record->m_nAncStdPrice != record->m_nMecaStdPrice)
    {
      MyMessageBox("date error", "m_tblEffectData[tbl_code_res].. price..");
      return false;
    }
  }

  if (m_tblEffectData[0].GetRecordNum() > 77)
  {
    MyMessageBox(
      "date error",
      "m_tblEffectData[effect_code_skill].ReadRecordNum() > max_skill_num + max_item_skill_num + max_race_buff_skill_num "
      "+ max_aura_skill_num + max_monster_skill_num");
    return false;
  }
  if (m_tblEffectData[1].GetRecordNum() > 88)
  {
    MyMessageBox("date error", "m_tblEffectData[effect_code_force].ReadRecordNum() > force_storage_num");
    return false;
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblMonster.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _monster_fld *record = reinterpret_cast<_monster_fld *>(m_tblMonster.GetRecord(n));
    if (record->m_fLevel <= 0.0f)
    {
      MyMessageBox("date error", "m_tblMonster, %d Rec.. Level == %d", n, (int)record->m_fLevel);
      return false;
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblEffectData[0].GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _skill_fld *record = reinterpret_cast<_skill_fld *>(m_tblEffectData[0].GetRecord(n));
    if (record->m_nMastIndex >= 8u)
    {
      dataError = true;
      MyMessageBox("date error", "skill mastery error( %s : %d)", record->m_strCode, record->m_nMastIndex);
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblEffectData[1].GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _force_fld *record = reinterpret_cast<_force_fld *>(m_tblEffectData[1].GetRecord(n));
    if (record->m_nMastIndex >= 0x18u)
    {
      dataError = true;
      MyMessageBox("date error", "force mastery error( %s : %d)", record->m_strCode, record->m_nMastIndex);
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblEffectData[2].GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    (void)reinterpret_cast<_skill_fld *>(m_tblEffectData[2].GetRecord(n));
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblUnitFrame.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _UnitFrame_fld *record = reinterpret_cast<_UnitFrame_fld *>(m_tblUnitFrame.GetRecord(n));
    if (record->m_nMoney > 6u)
    {
      MyMessageBox("date error", "m_tblUnitFrame, %d Rec.. MoneyCode == %d", n, record->m_nMoney);
      return false;
    }
  }

  int j = 0;
  for (j = 0; j < 6; ++j)
  {
    for (int n = 0; ; ++n)
    {
      const int recordNum = static_cast<int>(m_tblUnitPart[j].GetRecordNum());
      if (n >= recordNum)
      {
        break;
      }
      _UnitPart_fld *record = reinterpret_cast<_UnitPart_fld *>(m_tblUnitPart[j].GetRecord(n));
      if (record->m_nMoney > 6u)
      {
        MyMessageBox("date error", "m_tblUnitPart[%d], %d Rec.. MoneyCode == %d", j, n, record->m_nMoney);
        return false;
      }
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblUnitBullet.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _UnitBullet_fld *record = reinterpret_cast<_UnitBullet_fld *>(m_tblUnitBullet.GetRecord(n));
    if (record->m_nMoney > 6u)
    {
      MyMessageBox("date error", "m_tblUnitBullet[%d], %d Rec.. MoneyCode == %d", j, n, record->m_nMoney);
      return false;
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblItemData[24].GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    (void)reinterpret_cast<_AnimusItem_fld *>(m_tblItemData[24].GetRecord(n));
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(CQuestMgr::s_tblQuest->GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _Quest_fld *record = reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(n));
    const int len = static_cast<int>(std::strlen(record->m_strCode));
    if (len > 7)
    {
      MyMessageBox(
        "date error",
        "CQuestMgr::s_tblQuest, %d Rec.. code: %s size(%d) is biger than quest_header_code_len(%d)",
        n,
        record->m_strCode,
        len,
        7);
      return false;
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblItemMakeData.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    (void)reinterpret_cast<_ItemMakeData_fld *>(m_tblItemMakeData.GetRecord(n));
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblClass.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _class_fld *record = reinterpret_cast<_class_fld *>(m_tblClass.GetRecord(n));
    if (record->m_nClass < 4u)
    {
      for (int k = 0; k < 9; ++k)
      {
        char *itemCode = record->m_DefaultItem[k].strDefaultItem;
        if (std::strncmp(itemCode, "-1", 2) != 0)
        {
          const int tableCode = GetItemTableCode(itemCode);
          if (tableCode == -1)
          {
            MyMessageBox(
              "data error",
              "class-reward-item error .., class: %s, table code error %s",
              record->m_strCode,
              itemCode);
            dataError = true;
          }
          else if (tableCode == 19)
          {
            MyMessageBox("data error", "class-reward-item error.., class: %s, unit key %s", record->m_strCode, itemCode);
            dataError = true;
          }
          else if (!g_Main.m_tblItemData[tableCode].GetRecord(itemCode))
          {
            MyMessageBox(
              "data error",
              "class-reward-item error.., class: %s, unregistered item %s",
              record->m_strCode,
              itemCode);
            dataError = true;
          }
        }
      }
    }
    else
    {
      MyMessageBox("data error", "classcode (%s : %d).., code error error", record->m_strCode, record->m_nClass);
      dataError = true;
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(CQuestMgr::s_tblQuest->GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _Quest_fld *record = reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(n));
    for (int m = 0; m < 6; ++m)
    {
      char *itemCode = record->m_RewardItem[m].m_strConsITCode;
      if (std::strncmp(itemCode, "-1", 2) != 0)
      {
        const int tableCode = GetItemTableCode(itemCode);
        if (tableCode == -1)
        {
          MyMessageBox(
            "data error",
            "quest-reward-item error.., quest: %s, table code error %s",
            record->m_strCode,
            itemCode);
          dataError = true;
        }
        else if (!g_Main.m_tblItemData[tableCode].GetRecord(itemCode))
        {
          MyMessageBox(
            "data error",
            "quest-reward-item.., quest: %s, unregistered item %s",
            record->m_strCode,
            itemCode);
          dataError = true;
        }
      }
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblEditData.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _EditData_fld *record = reinterpret_cast<_EditData_fld *>(m_tblEditData.GetRecord(n));
    for (int ii = 0; ii < 30; ++ii)
    {
      (void)record->m_Node[ii];
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblItemData[10].GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _BulletItem_fld *record = reinterpret_cast<_BulletItem_fld *>(m_tblItemData[10].GetRecord(n));
    if (record->m_nDurUnit > 0xFFFF || record->m_nDurUnit < 1)
    {
      MyMessageBox("data error", "bullet's duration error(1~65535).., %s : %d", record->m_strCode, record->m_nDurUnit);
      dataError = true;
    }
  }

  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(m_tblMonster.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    _monster_fld *record = reinterpret_cast<_monster_fld *>(m_tblMonster.GetRecord(n));
    const int len = static_cast<int>(std::strlen(record->m_strCode));
    if (len > 5)
    {
      MyMessageBox(
        "data error",
        "monster code string size error(monster_code_len: %d != %d).., %s",
        5,
        len,
        record->m_strCode);
      dataError = true;
    }
  }

  if (!ItemCombineMgr::CheckLoadData())
  {
    dataError = true;
  }

  if (!CSUItemSystem::Instance()->SUItemSystem_CheckData())
  {
    MyMessageBox("Data Check Error", "Set&Unique Item Data Check Error");
    dataError = true;
  }

  return !dataError;
}

void CMainThread::gm_MainThreadControl()
{
  m_nSleepTerm = GetPrivateProfileIntA("MainThread", "SleepTerm", 2, ".\\Initialize\\WorldSystem.ini");
  m_nSleepValue = GetPrivateProfileIntA("MainThread", "SleepValue", 1, ".\\Initialize\\WorldSystem.ini");
  m_nSleepIgnore = GetPrivateProfileIntA("MainThread", "SleepIgnore", 0, ".\\Initialize\\WorldSystem.ini");

  if (m_nSleepTerm < 0)
  {
    m_nSleepTerm = 0;
  }
  if (m_nSleepValue < 0)
  {
    m_nSleepValue = 0;
  }
  if (m_nSleepValue > 10)
  {
    m_nSleepValue = 10;
  }
  if (m_nSleepIgnore >= 2)
  {
    m_nSleepIgnore = 0;
  }

  _ATTACK_DELAY_CHECKER::s_nSpareTime = GetPrivateProfileIntA(
    "Rule",
    "AttackSpareDelay",
    100,
    ".\\Initialize\\WorldSystem.ini");

  m_nLimUserNum = GetPrivateProfileIntA("System", "LimUserNum", MAX_PLAYER, ".\\Initialize\\WorldSystem.ini");
  if (m_nLimUserNum > MAX_PLAYER)
  {
    m_nLimUserNum = MAX_PLAYER;
  }

  char returnedString[32]{};
  GetPrivateProfileStringA("System", "CheckSum", "TRUE", returnedString, sizeof(returnedString), ".\\Initialize\\WorldSystem.ini");
  m_bCheckSumActive = (strcmp(returnedString, "TRUE") == 0);

  if (g_pFrame != nullptr)
  {
    g_pFrame->SendMessage(0x000C, 0, 0);
  }
}

bool CMainThread::ObjectInit()
{
  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    g_UserDB[index].Init(index);
  }
  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    g_PartyPlayer[index].Init(index);
  }
  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    _object_id id(0, 0, static_cast<unsigned __int16>(index));
    g_Player[index].Init(&id);
  }

  g_Monster = static_cast<CMonster *>(operator new[](sizeof(CMonster) * MAX_MONSTER));
  if (g_Monster == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_MONSTER; ++index)
  {
    _object_id id(0, 1, static_cast<unsigned __int16>(index));
    g_Monster[index].Init(&id);
  }

  g_NPC = new CMerchant[MAX_NPC];
  if (g_NPC == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_NPC; ++index)
  {
    _object_id id(0, 2, static_cast<unsigned __int16>(index));
    g_NPC[index].Init(&id);
  }

  g_Animus = new CAnimus[MAX_ANIMUS];
  if (g_Animus == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_ANIMUS; ++index)
  {
    _object_id id(0, 3, static_cast<unsigned __int16>(index));
    g_Animus[index].Init(&id);
  }

  g_Tower = new CGuardTower[MAX_TOWER];
  if (g_Tower == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_TOWER; ++index)
  {
    _object_id id(0, 4, static_cast<unsigned __int16>(index));
    g_Tower[index].Init(&id);
  }

  g_Stone = new CHolyStone[MAX_STONE];
  if (g_Stone == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_STONE; ++index)
  {
    _object_id id(0, 5, static_cast<unsigned __int16>(index));
    g_Stone[index].Init(&id);
  }

  g_Keeper = new CHolyKeeper[MAX_KEEPER];
  if (g_Keeper == nullptr)
  {
    return false;
  }
  {
    _object_id id(0, 6, 0);
    g_Keeper[0].Init(&id);
  }

  g_Trap = new CTrap[MAX_TRAP];
  if (g_Trap == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_TRAP; ++index)
  {
    _object_id id(0, 7, static_cast<unsigned __int16>(index));
    g_Trap[index].Init(&id);
  }

  g_ItemBox = new CItemBox[MAX_ITEMBOX];
  if (g_ItemBox == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_ITEMBOX; ++index)
  {
    _object_id id(1, 0, static_cast<unsigned __int16>(index));
    g_ItemBox[index].Init(&id);
  }

  g_ParkingUnit = new CParkingUnit[MAX_PARKING_UNIT];
  if (g_ParkingUnit == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_PARKING_UNIT; ++index)
  {
    _object_id id(1, 2, static_cast<unsigned __int16>(index));
    g_ParkingUnit[index].Init(&id);
  }

  g_DarkHole = new CDarkHole[MAX_DARKHOLE];
  if (g_DarkHole == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_DARKHOLE; ++index)
  {
    _object_id id(1, 1, static_cast<unsigned __int16>(index));
    g_DarkHole[index].Init(&id);
  }

  g_Guild = new CGuild[MAX_GUILD];
  if (g_Guild == nullptr)
  {
    return false;
  }
  for (unsigned int index = 0; index < MAX_GUILD; ++index)
  {
    g_Guild[index].Init(index);
  }

  CRaceBossMsgController::Instance()->Init();
  CReturnGateController::Instance()->Init(0x7E);
  CRecallEffectController::Instance()->Init(0xFD);

  CPlayer::SetStaticMember();
  CAnimus::SetStaticMember();
  CAttack::SetStaticMember(m_tblEffectData);
  _WEAPON_PARAM::SetStaticMember(&m_tblItemData[6]);
  _MASTERY_PARAM::SetStaticMember(m_tblEffectData, &m_tblEffectData[1]);

  m_listDQSData.SetList(MAX_DQS);
  m_listDQSDataComplete.SetList(MAX_DQS);
  m_listDQSDataEmpty.SetList(MAX_DQS);
  for (unsigned int index = 0; index < MAX_DB_QRY_SYN; ++index)
  {
    m_listDQSDataEmpty.PushNode_Back(index);
  }

  m_pRFEvent_ClassRefine = new RFEvent_ClassRefine();
  if (m_pRFEvent_ClassRefine == nullptr)
  {
    return false;
  }
  if (!static_cast<RFEvent_ClassRefine *>(m_pRFEvent_ClassRefine)->Initialzie())
  {
    return false;
  }

  if (!LendItemMng::Instance()->Initialize())
  {
    MyMessageBox("ObjectInit() Error", "Lend item manager failed initialzie.");
    return false;
  }
  if (!CNuclearBombMgr::Instance()->MissileInit())
  {
    MyMessageBox("ObjectInit() Error", "Nuclear Missile failed initalize");
    return false;
  }
  if (!TimeLimitJadeMng::Instance()->Init())
  {
    MyMessageBox("ObjectInit() Error", "TimeLimitJadeMng failed initalize");
    return false;
  }
  if (!CPvpCashMng::Instance()->LoadData())
  {
    MyMessageBox("ObjectInit() Error", "CPvpCashMng::Instance()->LoadData() failed initalize");
    return false;
  }
  if (!DfAIMgr::OnUsStateTBLInit())
  {
    MyMessageBox("ObjectInit() Error", " !DfAIMgr::OnUsStateTBLInit() ");
    return false;
  }

  g_MonsterEventSet = CMonsterEventSet();
  return true;
}

bool CMainThread::NetworkInit()
{
  _NET_TYPE_PARAM params[4]{};

  params[0].m_bServer = 1;
  params[0].m_wSocketMaxNum = MAX_PLAYER;
  params[0].m_bRealSockCheck = 1;
  params[0].m_bSystemLogFile = 1;
  if (m_bReleaseServiceMode)
  {
    params[0].m_byRecvThreadNum = 8;
  }
  params[0].m_byRecvSleepTime = 10;
  params[0].m_bySendSleepTime = 2;
  params[0].m_wPort = 27780;
  params[0].m_bSpeedHackCheck = 1;
  params[0].m_bOddMsgWriteLog = 1;
  params[0].m_dwSocketRecycleTerm = 30000;
  params[0].m_bOddMsgDisconnect = 1;
  params[0].m_dwProcessMsgNumPerLoop = 20;
  if (m_bReleaseServiceMode)
  {
    params[0].m_dwSendBufferSize = 40000;
    params[0].m_dwSendSafeSize = 10000;
  }
  params[0].m_bSendSafe = true;
  strcpy_s(params[0].m_szModuleName, sizeof(params[0].m_szModuleName), "ClientLine");

  params[1].m_bServer = 1;
  params[1].m_wPort = 27555;
  params[1].m_wSocketMaxNum = 2;
  params[1].m_bRealSockCheck = 1;
  params[1].m_bSystemLogFile = 1;
  params[1].m_bRecvLogFile = 1;
  params[1].m_bSendLogFile = 1;
  params[1].m_bOddMsgWriteLog = 1;
  params[1].m_bOddMsgDisconnect = 1;
  params[1].m_bSendSafe = true;
  params[1].m_byRecvSleepTime = 10;
  params[1].m_bySendSleepTime = 2;
  strcpy_s(params[1].m_szModuleName, sizeof(params[1].m_szModuleName), "AccountLine");

  params[2].m_bServer = 1;
  params[2].m_wPort = 27556;
  params[2].m_wSocketMaxNum = 1;
  params[2].m_bRealSockCheck = 1;
  params[2].m_bSystemLogFile = 1;
  params[2].m_bRecvLogFile = 1;
  params[2].m_bSendLogFile = 1;
  params[2].m_bOddMsgWriteLog = 1;
  params[2].m_bOddMsgDisconnect = 1;
  params[2].m_dwSendBufferSize = 1000000;
  params[2].m_dwRecvBufferSize = 1000000;
  params[2].m_bSendSafe = true;
  params[2].m_byRecvSleepTime = 10;
  params[2].m_bySendSleepTime = 2;
  strcpy_s(params[2].m_szModuleName, sizeof(params[2].m_szModuleName), "WebLine");

  params[3].m_bServer = 1;
  params[3].m_wPort = 0;
  params[3].m_wSocketMaxNum = 1;
  params[3].m_bRealSockCheck = 1;
  params[3].m_bSystemLogFile = 1;
  params[3].m_bRecvLogFile = 1;
  params[3].m_bSendLogFile = 1;
  params[3].m_bOddMsgWriteLog = 1;
  params[3].m_bOddMsgDisconnect = 1;
  params[3].m_dwSendBufferSize = 1000000;
  params[3].m_dwRecvBufferSize = 1000000;
  params[3].m_bSendSafe = true;
  params[3].m_byRecvSleepTime = 10;
  params[3].m_bySendSleepTime = 2;
  strcpy_s(params[3].m_szModuleName, sizeof(params[3].m_szModuleName), "BillingLine");

  g_Network.m_dwUseProcessNum = 4;
  strcpy_s(g_Network.m_szSystemName, sizeof(g_Network.m_szSystemName), "GameServer");
  strcpy_s(g_Network.m_szLogPath, sizeof(g_Network.m_szLogPath), "..\\ZoneServerLog\\NetLog");
  for (unsigned int i = 0; i < 4; ++i)
  {
    g_Network.m_pProcess[i] = &g_Network.m_Process[i];
    g_Network.m_Process[i].m_Type = params[i];
  }
  AddPassablePacket();

  return true;
}

void CMainThread::MakeSystemTower()
{
  const char *raceNames[3] = {"BELLATO", "CORA", "ACCRETIA"};
  for (unsigned int race = 0; race < 3; ++race)
  {
    for (int nIniIndex = 0; nIniIndex < 50; ++nIniIndex)
    {
      char mapKey[160]{};
      char mapCode[152]{};
      sprintf_s(mapKey, sizeof(mapKey), "Map%d", nIniIndex);
      GetPrivateProfileStringA(raceNames[race], mapKey, "NULL", mapCode, sizeof(mapCode), ".\\Script\\SystemGuardTower.ini");
      if (std::strcmp(mapCode, "NULL") == 0)
      {
        continue;
      }

      CMapData *map = g_MapOper.GetMap(mapCode);
      if (!map)
      {
        WritePrivateProfileStringA(raceNames[race], mapKey, "NULL", ".\\Script\\SystemGuardTower.ini");
        m_logLoadingError.Write("SystemTower: invalid map (%s) for race %u idx %d", mapCode, race, nIniIndex);
        continue;
      }

      char posKey[160]{};
      char posValue[136]{};
      float pos[3]{};
      bool posValid = true;

      sprintf_s(posKey, sizeof(posKey), "Pos%d_x", nIniIndex);
      GetPrivateProfileStringA(raceNames[race], posKey, "NULL", posValue, sizeof(posValue), ".\\Script\\SystemGuardTower.ini");
      if (std::strcmp(posValue, "NULL") == 0)
      {
        posValid = false;
      }
      else
      {
        pos[0] = static_cast<float>(std::atof(posValue));
      }

      sprintf_s(posKey, sizeof(posKey), "Pos%d_y", nIniIndex);
      GetPrivateProfileStringA(raceNames[race], posKey, "NULL", posValue, sizeof(posValue), ".\\Script\\SystemGuardTower.ini");
      if (std::strcmp(posValue, "NULL") == 0)
      {
        posValid = false;
      }
      else
      {
        pos[1] = static_cast<float>(std::atof(posValue));
      }

      sprintf_s(posKey, sizeof(posKey), "Pos%d_z", nIniIndex);
      GetPrivateProfileStringA(raceNames[race], posKey, "NULL", posValue, sizeof(posValue), ".\\Script\\SystemGuardTower.ini");
      if (std::strcmp(posValue, "NULL") == 0)
      {
        posValid = false;
      }
      else
      {
        pos[2] = static_cast<float>(std::atof(posValue));
      }

      if (!posValid || !map->IsMapIn(pos))
      {
        WritePrivateProfileStringA(raceNames[race], mapKey, "NULL", ".\\Script\\SystemGuardTower.ini");
        m_logLoadingError.Write("SystemTower: invalid position for race %u idx %d", race, nIniIndex);
        continue;
      }

      char codeKey[160]{};
      char recordCode[152]{};
      sprintf_s(codeKey, sizeof(codeKey), "Code%d", nIniIndex);
      GetPrivateProfileStringA(raceNames[race], codeKey, "NULL", recordCode, sizeof(recordCode), ".\\Script\\SystemGuardTower.ini");
      if (std::strcmp(recordCode, "NULL") == 0)
      {
        WritePrivateProfileStringA(raceNames[race], mapKey, "NULL", ".\\Script\\SystemGuardTower.ini");
        m_logLoadingError.Write("SystemTower: missing record code for race %u idx %d", race, nIniIndex);
        continue;
      }

      _base_fld *record = g_Main.m_tblItemData[25].GetRecord(recordCode);
      if (!record)
      {
        WritePrivateProfileStringA(raceNames[race], mapKey, "NULL", ".\\Script\\SystemGuardTower.ini");
        m_logLoadingError.Write("SystemTower: record not found (%s) race %u idx %d", recordCode, race, nIniIndex);
        continue;
      }
    }
  }
}

void CMainThread::AddPassablePacket()
{
  g_Network.SetPassablePacket(0, 4u, 3u);
  g_Network.SetPassablePacket(0, 4u, 4u);
  g_Network.SetPassablePacket(0, 4u, 5u);
  g_Network.SetPassablePacket(0, 4u, 6u);
  g_Network.SetPassablePacket(0, 4u, 7u);
  g_Network.SetPassablePacket(0, 4u, 0xB1u);
  g_Network.SetPassablePacket(0, 4u, 0x12u);
  g_Network.SetPassablePacket(0, 4u, 0x14u);
  g_Network.SetPassablePacket(0, 4u, 0x19u);
  g_Network.SetPassablePacket(0, 4u, 0x1Eu);
  g_Network.SetPassablePacket(0, 5u, 7u);
  g_Network.SetPassablePacket(0, 5u, 8u);
  g_Network.SetPassablePacket(0, 5u, 9u);
  g_Network.SetPassablePacket(0, 5u, 0xAu);
  g_Network.SetPassablePacket(0, 5u, 0xBu);
  g_Network.SetPassablePacket(0, 5u, 0xCu);
  g_Network.SetPassablePacket(0, 5u, 0xDu);
  g_Network.SetPassablePacket(0, 5u, 0xEu);
  g_Network.SetPassablePacket(0, 5u, 0xFu);
  g_Network.SetPassablePacket(0, 5u, 0x97u);
  g_Network.SetPassablePacket(0, 5u, 0x98u);
  g_Network.SetPassablePacket(0, 2u, 0xAu);
  g_Network.SetPassablePacket(0, 2u, 0xBu);
  g_Network.SetPassablePacket(0, 2u, 0xBu);
  g_Network.SetPassablePacket(0, 0xDu, 0x1Du);
  g_Network.SetPassablePacket(0, 0xEu, 7u);
  g_Network.SetPassablePacket(0, 0xEu, 8u);
  g_Network.SetPassablePacket(0, 0xEu, 0x19u);
  g_Network.SetPassablePacket(0, 0xEu, 0x3Cu);
  g_Network.SetPassablePacket(0, 0xEu, 0x42u);
  g_Network.SetPassablePacket(0, 0xEu, 0x37u);
  g_Network.SetPassablePacket(0, 0xEu, 0x44u);
  g_Network.SetPassablePacket(0, 0x38u, 6u);
  g_Network.SetPassablePacket(0, 0x10u, 0x14u);
  g_Network.SetPassablePacket(0, 0x10u, 0x15u);
  g_Network.SetPassablePacket(0, 0x10u, 0x16u);
  g_Network.SetPassablePacket(0, 0x10u, 0x18u);
  g_Network.SetPassablePacket(0, 0x10u, 0x19u);
  g_Network.SetPassablePacket(0, 0x10u, 0x19u);
  g_Network.SetPassablePacket(0, 0x11u, 3u);
  g_Network.SetPassablePacket(0, 0x11u, 6u);
  g_Network.SetPassablePacket(0, 0x11u, 9u);
  g_Network.SetPassablePacket(0, 0x11u, 9u);
  g_Network.SetPassablePacket(0, 0x11u, 0x11u);
  g_Network.SetPassablePacket(0, 0x11u, 0x1Au);
}

void __cdecl CMainThread::RuleThread(void *param)
{
  CMainThread *self = static_cast<CMainThread *>(param);
  if (self == nullptr)
  {
    _endthreadex(0);
    return;
  }

  srand(static_cast<unsigned int>(time(nullptr)));
  int sleepCounter = 0;
  while (self->m_bRuleThread)
  {
    (void)GetTickCount();
    if (!self->m_nSleepIgnore && ++sleepCounter > self->m_nSleepTerm)
    {
      Sleep(self->m_nSleepValue);
      sleepCounter = 0;
    }
  }
  _endthreadex(0);
}

void __cdecl CMainThread::DQSThread(void *param)
{
  CMainThread *self = static_cast<CMainThread *>(param);
  if (self == nullptr)
  {
    _endthreadex(0);
    return;
  }

  char logPath[128]{};
  const unsigned int now = GetKorLocalTime();
  sprintf_s(logPath, "..\\ZoneServerLog\\Systemlog\\DQSError%d.log", now);
  self->m_logDQS.SetWriteLogFile(logPath, 1, 0, 1, 1);

  int throttle = 0;
  while (self->m_bDQSThread)
  {
    if (throttle++ > 100)
    {
      Sleep(1);
      throttle = 0;
    }
  }
  _endthreadex(0);
}
