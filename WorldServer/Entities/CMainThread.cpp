#include "pch.h"

#include "CMainThread.h"

#include <crtdbg.h>
#include <process.h>
#include <cstdio>
#include <cstring>

#include "AutominePersonalMgr.h"
#include "CActionPointSystemMgr.h"
#include "CAsyncLogger.h"
#include "CBillingManager.h"
#include "CBossMonsterScheduleSystem.h"
#include "CCashDBWorkManager.h"
#include "CExchangeEvent.h"
#include "CGoldenBoxItemMgr.h"
#include "CGuildBattleController.h"
#include "CGuildRoomSystem.h"
#include "CHonorGuild.h"
#include "CLuaScriptMgr.h"
#include "CLogTypeDBTaskManager.h"
#include "CMapOperation.h"
#include "CMonsterEventRespawn.h"
#include "CMonsterEventSet.h"
#include "CMoveMapLimitManager.h"
#include "CPcBangFavor.h"
#include "CPostSystemManager.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CRaceBuffManager.h"
#include "CTotalGuildRankManager.h"
#include "CUnmannedTraderController.h"
#include "CWeeklyGuildRankManager.h"
#include "CashItemRemoteStore.h"
#include "CandidateMgr.h"
#include "CDarkHoleDungeonQuest.h"
#include "CCryptor.h"
#include "CRtc.h"
#include "CNationCodeStrTable.h"
#include "CNationSettingManager.h"
#include "KorLocalTime.h"
#include "PatriarchElectProcessor.h"
#include "TimeItem.h"
#include "WheatyExceptionReport.h"
#include "WorldServerUtil.h"
#include "cStaticMember_Player.h"

void Us_HFSM::__vtable_anchor() {}

Us_HFSM::~Us_HFSM() = default;

CMainThread g_Main;

CMainThread::CMainThread() = default;

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
}

void TimeLimitMgr::InitializeTLMgr()
{
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
  return true;
}

bool CMainThread::check_dbsyn_data_size()
{
  return true;
}

bool CMainThread::DataFileInit()
{
  return true;
}

bool CMainThread::ObjectInit()
{
  return true;
}

bool CMainThread::NetworkInit()
{
  return true;
}

void CMainThread::MakeSystemTower()
{
}

void __cdecl CMainThread::RuleThread(void *param)
{
  (void)param;
}

void __cdecl CMainThread::DQSThread(void *param)
{
  (void)param;
}
