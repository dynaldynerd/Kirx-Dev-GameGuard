#include "pch.h"

#include "CGuildBattleController.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CMapData.h"
#include "GuildBattle.h"
#include "GuildBattleTypes.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"
#include "guild_battle_get_gravity_stone_result_zocl.h"

#include <cstdlib>

CGuildBattleController *CGuildBattleController::Instance()
{
  static CGuildBattleController s_instance;
  return &s_instance;
}

CGuildBattleController::CGuildBattleController()
{
  // this is not a stub
}

CGuildBattleController::~CGuildBattleController()
{
  CleanUp();
}

void CGuildBattleController::CleanUp()
{
  GUILD_BATTLE::CGuildBattleLogger::Destroy();
  GUILD_BATTLE::CGuildBattleRankManager::Destroy();
  GUILD_BATTLE::CNormalGuildBattleFieldList::Destroy();
  GUILD_BATTLE::CGuildBattleScheduler::Destroy();
  GUILD_BATTLE::CPossibleBattleGuildListManager::Destroy();
  GUILD_BATTLE::CNormalGuildBattleManager::Destroy();
  GUILD_BATTLE::CGuildBattleReservedScheduleListManager::Destroy();
  GUILD_BATTLE::CCurrentGuildBattleInfoManager::Destroy();
  GUILD_BATTLE::CNormalGuildBattleStateListPool::Destroy();
}

void CGuildBattleController::Loop()
{
  GUILD_BATTLE::CGuildBattleScheduleManager *scheduleManager = GUILD_BATTLE::CGuildBattleScheduleManager::Instance();
  scheduleManager->Loop();

  GUILD_BATTLE::CNormalGuildBattleManager *battleManager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  battleManager->Loop();
}

void CGuildBattleController::Clear()
{
  GUILD_BATTLE::CNormalGuildBattleManager *battleManager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  battleManager->Clear();

  GUILD_BATTLE::CGuildBattleReservedScheduleListManager *reservedManager =
    GUILD_BATTLE::CGuildBattleReservedScheduleListManager::Instance();
  reservedManager->Clear();

  GUILD_BATTLE::CPossibleBattleGuildListManager *possibleListManager =
    GUILD_BATTLE::CPossibleBattleGuildListManager::Instance();
  possibleListManager->Clear();

  if (!SaveINI())
  {
    GUILD_BATTLE::CGuildBattleLogger::Instance()->Log("CGuildBattleController::Clear() SaveINI() Fail!");
  }
}

void CGuildBattleController::Flip()
{
  GUILD_BATTLE::CNormalGuildBattleManager *battleManager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  battleManager->Flip();
  battleManager->DoDayChangedWork();

  GUILD_BATTLE::CGuildBattleReservedScheduleListManager *reservedManager =
    GUILD_BATTLE::CGuildBattleReservedScheduleListManager::Instance();
  reservedManager->Flip();

  GUILD_BATTLE::CPossibleBattleGuildListManager *possibleListManager =
    GUILD_BATTLE::CPossibleBattleGuildListManager::Instance();
  possibleListManager->DoDayChangedWork();

  if (!SaveINI())
  {
    GUILD_BATTLE::CGuildBattleLogger::Instance()->Log("CGuildBattleController::Flip() SaveINI() Fail!");
  }
}

bool CGuildBattleController::Init()
{
  auto *logger = GUILD_BATTLE::CGuildBattleLogger::Instance();
  if (!logger->Init())
    return false;

  auto *rewardManager = GUILD_BATTLE::CGuildBattleRewardItemManager::Instance();
  if (!rewardManager->Init())
    return false;

  auto *rankManager = GUILD_BATTLE::CGuildBattleRankManager::Instance();
  if (!rankManager->Init())
    return false;

  auto *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();
  if (!fieldList->Init())
    return false;

  auto *scheduler = GUILD_BATTLE::CGuildBattleScheduler::Instance();
  if (!scheduler->Init())
    return false;

  auto *possibleList = GUILD_BATTLE::CPossibleBattleGuildListManager::Instance();
  if (!possibleList->Init())
    return false;

  auto *battleManager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  if (!battleManager->Init())
    return false;

  auto *reservedSchedule = GUILD_BATTLE::CGuildBattleReservedScheduleListManager::Instance();
  if (!reservedSchedule->Init())
    return false;

  auto *currentInfo = GUILD_BATTLE::CCurrentGuildBattleInfoManager::Instance();
  if (!currentInfo->Init())
    return false;

  auto *stateListPool = GUILD_BATTLE::CNormalGuildBattleStateListPool::Instance();
  return stateListPool->Init();
}

char CGuildBattleController::Load()
{
  auto *logger = GUILD_BATTLE::CGuildBattleLogger::Instance();
  const int curDay = GetCurDay();
  if (curDay < 0)
  {
    logger->Log("CGuildBattleController::Load(%d) ::GetCurDay() Fail!", curDay);
    return 0;
  }

  unsigned int uiMapCnt = 0;
  unsigned int iToday = 0;
  unsigned int iTodayDayID = 0;
  int iTomorrow = 0;
  int iTomorrowDayID = 1;

  if (!LoadINI(&uiMapCnt, &iToday, &iTodayDayID, &iTomorrow, &iTomorrowDayID))
  {
    logger->Log("CGuildBattleController::Load(%d) LoadINI() Fail!", curDay);
  }

  auto *rankManager = GUILD_BATTLE::CGuildBattleRankManager::Instance();
  if (!rankManager->Load())
  {
    logger->Log("CGuildBattleController::Load() CGuildBattleRankManager::Instance()->Load() Fail!");
    return 0;
  }

  auto *scheduleManager = GUILD_BATTLE::CGuildBattleScheduleManager::Instance();
  if (!scheduleManager->Load(curDay, uiMapCnt, iToday, iTodayDayID, iTomorrow, iTomorrowDayID))
  {
    logger->Log("CGuildBattleScheduleManager::Load() CGuildBattleScheduleManager::Instance()->Load() Fail!");
    return 0;
  }

  auto *battleManager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  if (!battleManager->Load(curDay, uiMapCnt, iToday, iTodayDayID, iTomorrow, iTomorrowDayID))
  {
    logger->Log("CGuildBattleController::Load() CNormalGuildBattleManager::Instance()->Load() Fail!");
    return 0;
  }

  auto *reservedManager = GUILD_BATTLE::CGuildBattleReservedScheduleListManager::Instance();
  if (!reservedManager->Load(curDay, uiMapCnt, iToday, iTomorrow))
  {
    logger->Log(
      "CGuildBattleController::Load() CGuildBattleReservedScheduleListManager::Instance()->Load() Fail!");
    return 0;
  }

  auto *possibleList = GUILD_BATTLE::CPossibleBattleGuildListManager::Instance();
  if (!possibleList->Load())
  {
    logger->Log("CGuildBattleController::Load() CPossibleBattleGuildListManager::Instance()->Load() Fail!");
    return 0;
  }

  if (!SaveINI())
  {
    logger->Log("CGuildBattleController::Load() SaveINI() Fail!");
    return 0;
  }

  return 1;
}

char CGuildBattleController::LoadINI(
  unsigned int *uiMapCnt,
  unsigned int *iToday,
  unsigned int *iTodayDayID,
  int *iTomorrow,
  int *iTomorrowDayID)
{
  *iToday = GetPrivateProfileIntA("GuildBattle", "Today", -1, "..\\SystemSave\\ServerState.ini");
  *iTodayDayID = GetPrivateProfileIntA("GuildBattle", "TodayDayID", 0, "..\\SystemSave\\ServerState.ini");
  *iTomorrow = GetPrivateProfileIntA("GuildBattle", "Tomorrow", -1, "..\\SystemSave\\ServerState.ini");
  *iTomorrowDayID = GetPrivateProfileIntA("GuildBattle", "TomorrowDayID", 1, "..\\SystemSave\\ServerState.ini");
  *uiMapCnt = GetPrivateProfileIntA("GuildBattle", "MapCnt", 0, "..\\SystemSave\\ServerState.ini");

  auto *logger = GUILD_BATTLE::CGuildBattleLogger::Instance();
  logger->Log(
    "CGuildBattleController::LoadINI(...) : iToday : %d, iTodayDayID : %d, iTommorow : %d, iTomorrowDayID : %d, uiMapCnt : %u Load!",
    *iToday,
    *iTodayDayID,
    *iTomorrow,
    *iTomorrowDayID,
    *uiMapCnt);
  return 1;
}

char CGuildBattleController::SaveINI()
{
  auto *scheduleManager = GUILD_BATTLE::CGuildBattleScheduleManager::Instance();
  auto *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();

  const int mapCnt = fieldList->GetMapCnt();
  const int today = GetCurDay();
  const int todayDayID = scheduleManager->GetTodayDayID();
  const int tomorrow = GetNextDay();
  const int tomorrowDayID = scheduleManager->GetTomorrowDayID();

  char buffer[260]{};
  if (!WritePrivateProfileStringA("GuildBattle", "Today", _itoa(today, buffer, 10), "..\\SystemSave\\ServerState.ini"))
  {
    return 0;
  }
  if (!WritePrivateProfileStringA(
        "GuildBattle",
        "TodayDayID",
        _itoa(todayDayID, buffer, 10),
        "..\\SystemSave\\ServerState.ini"))
  {
    return 0;
  }
  if (!WritePrivateProfileStringA(
        "GuildBattle",
        "Tomorrow",
        _itoa(tomorrow, buffer, 10),
        "..\\SystemSave\\ServerState.ini"))
  {
    return 0;
  }
  if (!WritePrivateProfileStringA(
        "GuildBattle",
        "TomorrowDayID",
        _itoa(tomorrowDayID, buffer, 10),
        "..\\SystemSave\\ServerState.ini"))
  {
    return 0;
  }
  if (!WritePrivateProfileStringA(
        "GuildBattle",
        "MapCnt",
        _ltoa(mapCnt, buffer, 10),
        "..\\SystemSave\\ServerState.ini"))
  {
    return 0;
  }

  auto *logger = GUILD_BATTLE::CGuildBattleLogger::Instance();
  logger->Log(
    "CGuildBattleController::SaveINI() : iToday : %d, iTodayDayID : %u, iTommorow : %d, iTomorrowDayID : %u, uiMapCnt : %u Save!",
    today,
    todayDayID,
    tomorrow,
    tomorrowDayID,
    mapCnt);
  return 1;
}

unsigned __int8 CGuildBattleController::IsAvailableSuggest(
  CGuild *pSrcGuild,
  unsigned int dwDestGuild,
  unsigned int dwStartTime,
  unsigned int dwNumber,
  unsigned int dwMapCode)
{
  if (pSrcGuild->m_dwSerial == dwDestGuild)
  {
    return 118;
  }
  if (10 * (dwNumber + 1) > 0x32)
  {
    return 126;
  }

  unsigned __int8 ret = pSrcGuild->SrcGuildIsAvailableBattleRequestState();
  if (ret)
  {
    return ret;
  }

  CGuild *destGuild = GetGuildDataFromSerial(g_Guild, MAX_GUILD, dwDestGuild);
  if (!destGuild)
  {
    return 111;
  }

  unsigned int mapIndex[9]{};
  auto *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();
  if (!fieldList->GetMapInx(pSrcGuild->m_byRace, dwMapCode, mapIndex))
  {
    return 120;
  }

  auto *scheduleManager = GUILD_BATTLE::CGuildBattleScheduleManager::Instance();
  ret = scheduleManager->IsEmptyTime(mapIndex[0], dwStartTime + 1);
  if (ret)
  {
    return ret;
  }

  if (pSrcGuild->m_byGrade < GUILD_BATTLE::LIMIT_SRC_GRADE)
  {
    return 113;
  }
  if (destGuild->m_byGrade < GUILD_BATTLE::LIMIT_DEST_GRADE)
  {
    return 114;
  }
  if (std::abs(pSrcGuild->m_byGrade - destGuild->m_byGrade) > 4)
  {
    return 115;
  }
  if (pSrcGuild->m_dTotalGold < 5000.0)
  {
    return 116;
  }
  if (pSrcGuild->m_byRace == destGuild->m_byRace)
  {
    return destGuild->DestGuildIsAvailableBattleRequestState();
  }
  return 117;
}

void CGuildBattleController::UpdatePossibleBattleGuildList()
{
  auto *manager = GUILD_BATTLE::CPossibleBattleGuildListManager::Instance();
  manager->UpdateGuildList();
}

void CGuildBattleController::JoinGuild(int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial)
{
  auto *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  manager->JoinGuild(n, dwGuildSerial, dwCharacSerial);
}

void CGuildBattleController::LeaveGuild(CPlayer *pkPlayer)
{
  if (pkPlayer && pkPlayer->m_bOper && pkPlayer->m_Param.m_pGuild
    && pkPlayer->m_Param.m_pGuild->m_dwSerial != static_cast<unsigned int>(-1))
  {
    auto *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
    manager->LeaveGuild(pkPlayer);
  }
}

unsigned __int8 CGuildBattleController::Add(
  CGuild *pSrcGuild,
  CGuild *pDestGuild,
  unsigned int dwStartTime,
  unsigned __int8 byNumber,
  unsigned int dwMapInx)
{
  return Add(pSrcGuild, pDestGuild, dwStartTime, 1u, byNumber, dwMapInx);
}

unsigned __int8 CGuildBattleController::Add(
  CGuild *pSrcGuild,
  CGuild *pDestGuild,
  unsigned int dwStartTime,
  unsigned int dwElapseTimeCnt,
  unsigned __int8 byNumber,
  unsigned int dwMapInx)
{
  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  return manager->Add(
    pSrcGuild,
    pDestGuild,
    dwStartTime + 1,
    dwElapseTimeCnt,
    static_cast<unsigned __int8>(10 * (byNumber + 1)),
    dwMapInx);
}

void CGuildBattleController::AddComplete(
  unsigned __int8 byResult,
  unsigned int uiMapID,
  unsigned int dwID,
  unsigned int dwSLID)
{
  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  manager->AddComplete(byResult, dwID);
  if (byResult)
  {
    GUILD_BATTLE::CGuildBattleScheduleManager *schedule = GUILD_BATTLE::CGuildBattleScheduleManager::Instance();
    schedule->ClearTommorowScheduleByID(uiMapID, dwID);
  }
  else
  {
    GUILD_BATTLE::CPossibleBattleGuildListManager *possibleList =
      GUILD_BATTLE::CPossibleBattleGuildListManager::Instance();
    possibleList->UpdateGuildList();
    GUILD_BATTLE::CGuildBattleReservedScheduleListManager *reserved =
      GUILD_BATTLE::CGuildBattleReservedScheduleListManager::Instance();
    reserved->PushDQS(uiMapID, dwSLID);
  }
}

unsigned __int8 CGuildBattleController::AddSchedule(char *szData)
{
  const auto *query = reinterpret_cast<const _qry_case_addguildbattleschedule *>(szData);
  g_Main.m_pWorldDB->SetAutoCommitMode(0);
  if (g_Main.m_pWorldDB->UpdateGuildBattleInfo(
        query->dwID,
        query->dw1PGuildSerial,
        query->dw2PGuildSerial,
        query->dwMapID,
        query->byBattleNumber)
      && g_Main.m_pWorldDB->UpdateGuildBattleScheduleInfo(
        query->dwID,
        query->dwSLID,
        query->byState,
        query->tStartTime,
        query->wTurmMin))
  {
    g_Main.m_pWorldDB->CommitTransaction();
    g_Main.m_pWorldDB->SetAutoCommitMode(1);
    return 0;
  }

  g_Main.m_pWorldDB->RollbackTransaction();
  g_Main.m_pWorldDB->SetAutoCommitMode(1);
  return 24;
}

bool CGuildBattleController::UpdateWinLose(
  unsigned __int8 byWinRace,
  unsigned int dwWinGuildSerial,
  unsigned __int8 byLoseRace,
  unsigned int dwLoseGuildSerial)
{
  GUILD_BATTLE::CGuildBattleRankManager *rankManager = GUILD_BATTLE::CGuildBattleRankManager::Instance();
  return rankManager->UpdateWinLose(byWinRace, dwWinGuildSerial, byLoseRace, dwLoseGuildSerial);
}

bool CGuildBattleController::UpdateDraw(
  unsigned __int8 by1PRace,
  unsigned int dw1PGuildSerial,
  unsigned __int8 by2PRace,
  unsigned int dw2PGuildSerial)
{
  GUILD_BATTLE::CGuildBattleRankManager *rankManager = GUILD_BATTLE::CGuildBattleRankManager::Instance();
  return rankManager->UpdateDraw(by1PRace, dw1PGuildSerial, by2PRace, dw2PGuildSerial);
}

bool CGuildBattleController::UpdateRank(unsigned __int8 byRace, unsigned __int8 *byOutData)
{
  GUILD_BATTLE::CGuildBattleRankManager *rankManager = GUILD_BATTLE::CGuildBattleRankManager::Instance();
  return rankManager->SelectGuildBattleRankList(
    byRace,
    reinterpret_cast<_worlddb_guild_battle_rank_list *>(byOutData));
}

void CGuildBattleController::CompleteUpdateRank(unsigned __int8 byResult, unsigned __int8 byRace, unsigned __int8 *pLoadData)
{
  if (byResult)
  {
    GUILD_BATTLE::CGuildBattleLogger *logger = GUILD_BATTLE::CGuildBattleLogger::Instance();
    logger->Log(
      "CGuildBattleController::CompleteUpdateRank( BYTE byResult(%u), BYTE byRace(%u), BYTE * pLoadData ) : Fail!",
      byResult,
      byRace);
    return;
  }

  GUILD_BATTLE::CGuildBattleRankManager *rankManager = GUILD_BATTLE::CGuildBattleRankManager::Instance();
  rankManager->Update(byRace, pLoadData);
}

bool CGuildBattleController::UpdateClearRerservedDayInfo(
  unsigned int dwStartSLID,
  unsigned int dwEndSLID,
  unsigned int dwStartSID,
  unsigned int dwEndSID)
{
  GUILD_BATTLE::CGuildBattleScheduler *scheduler = GUILD_BATTLE::CGuildBattleScheduler::Instance();
  if (!scheduler->UpdateClearGuildBattleScheduleDayInfo(dwStartSLID, dwEndSLID))
  {
    return false;
  }
  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  return manager->UpdateClearGuildBattleDayInfo(dwStartSID, dwEndSID);
}

bool CGuildBattleController::UpdateReservedGuildBattleSchedule(unsigned int dwSLID, unsigned __int8 *byOutData)
{
  GUILD_BATTLE::CGuildBattleReservedScheduleListManager *reserved =
    GUILD_BATTLE::CGuildBattleReservedScheduleListManager::Instance();
  return reserved->UpdateReservedShedule(
    dwSLID,
    reinterpret_cast<_worlddb_guild_battle_reserved_schedule_info *>(byOutData));
}

void CGuildBattleController::CompleteUpdateReservedSchedule(unsigned int dwMapID, unsigned __int8 *pLoadData)
{
  GUILD_BATTLE::CGuildBattleReservedScheduleListManager *reserved =
    GUILD_BATTLE::CGuildBattleReservedScheduleListManager::Instance();
  reserved->UpdateTomorrowComplete(
    dwMapID,
    reinterpret_cast<_worlddb_guild_battle_reserved_schedule_info *>(pLoadData));
}

void CGuildBattleController::CompleteCreateGuildBattleRankTable(unsigned __int8 byResult)
{
  const char *result = byResult ? "Fail" : "Success";
  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  ranking->Log(
    "CGuildBattleController::CompleteCreateGuildBattleRankTable( BYTE byResult(%u) ) : Create %s!",
    byResult,
    result);
}

void CGuildBattleController::CompleteClearGuildBattleRank(unsigned __int8 byResult)
{
  GUILD_BATTLE::CGuildBattleRankManager *rankManager = GUILD_BATTLE::CGuildBattleRankManager::Instance();
  rankManager->Clear();
  const char *result = byResult ? "Fail" : "Success";
  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  ranking->Log(
    "CGuildBattleController::ClearGuildBattleRankComplete( BYTE byResult(%u) ) : Clear %s!",
    byResult,
    result);
}

void CGuildBattleController::PushCreateGuildBattleRankTable()
{
  GUILD_BATTLE::CGuildBattleRankManager *manager = GUILD_BATTLE::CGuildBattleRankManager::Instance();
  manager->PushCreateGuildBattleRankTable();
}

void CGuildBattleController::PushClearGuildBattleRank()
{
  GUILD_BATTLE::CGuildBattleRankManager *manager = GUILD_BATTLE::CGuildBattleRankManager::Instance();
  manager->PushClearGuildBattleRank();
}

unsigned __int8 CGuildBattleController::Start(CPlayer *pkPlayer)
{
  if (!pkPlayer)
  {
    return static_cast<unsigned __int8>(-114);
  }

  unsigned int guildSerial = static_cast<unsigned int>(-1);
  if (pkPlayer->m_Param.m_pGuild)
  {
    guildSerial = pkPlayer->m_Param.m_pGuild->m_dwSerial;
  }

  if (guildSerial == static_cast<unsigned int>(-1))
  {
    return static_cast<unsigned __int8>(-115);
  }

  const int playerIndex = pkPlayer->m_ObjID.m_wIndex;
  const unsigned int charSerial = pkPlayer->m_pUserDB->m_dwSerial;
  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  manager->JoinGuild(playerIndex, guildSerial, charSerial);
  manager->LogIn(playerIndex, guildSerial, charSerial);
  return 0;
}

void CGuildBattleController::LogIn(CPlayer *pkPlayer)
{
  const int n = pkPlayer->m_ObjID.m_wIndex;
  const unsigned int dwGuildSerial = pkPlayer->m_Param.m_pGuild->m_dwSerial;
  const unsigned int dwCharacSerial = pkPlayer->m_pUserDB->m_dwSerial;
  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  manager->LogIn(n, dwGuildSerial, dwCharacSerial);
}

void CGuildBattleController::NetClose(CPlayer *pkPlayer)
{
  if (!pkPlayer)
  {
    return;
  }

  (void)pkPlayer->m_Param.GetRaceCode();
  unsigned int guildSerial = static_cast<unsigned int>(-1);
  if (pkPlayer->m_Param.m_pGuild)
  {
    guildSerial = pkPlayer->m_Param.m_pGuild->m_dwSerial;
  }

  const unsigned int charSerial = pkPlayer->m_pUserDB->m_dwSerial;
  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  const unsigned __int8 result = manager->NetClose(guildSerial, charSerial, pkPlayer);

  if (pkPlayer->m_bInGuildBattle && result)
  {
    const char *charName = pkPlayer->m_Param.GetCharNameW();
    GUILD_BATTLE::CGuildBattleLogger *logger = GUILD_BATTLE::CGuildBattleLogger::Instance();
    logger->Log(
      "CGuildBattleController::NetClose( %s ) : CNormalGuildBattleManager::Instance()->NetClose( %u, %u ) Return(%u) Fail!",
      charName,
      guildSerial,
      charSerial,
      result);
  }
}

void CGuildBattleController::SendPossibleBattleGuildList(
  int n,
  unsigned __int8 byRace,
  unsigned __int8 byPage,
  unsigned int dwVer)
{
  GUILD_BATTLE::CPossibleBattleGuildListManager *manager = GUILD_BATTLE::CPossibleBattleGuildListManager::Instance();
  manager->SendInfo(n, byRace, byPage, dwVer);
}

void CGuildBattleController::SendRankList(
  int n,
  unsigned __int8 bySelfRace,
  unsigned int dwCurVer,
  unsigned __int8 uiMapID,
  unsigned __int8 byPage,
  unsigned int dwGuildSerial)
{
  GUILD_BATTLE::CGuildBattleRankManager *manager = GUILD_BATTLE::CGuildBattleRankManager::Instance();
  manager->Send(n, bySelfRace, dwCurVer, static_cast<char>(uiMapID), byPage, dwGuildSerial);
}

void CGuildBattleController::Join(CPlayer *pkPlayer)
{
  if (!pkPlayer)
  {
    return;
  }

  const int n = pkPlayer->m_ObjID.m_wIndex;
  unsigned int guildSerial = static_cast<unsigned int>(-1);
  if (pkPlayer->m_Param.m_pGuild)
  {
    guildSerial = pkPlayer->m_Param.m_pGuild->m_dwSerial;
  }
  const unsigned int charSerial = pkPlayer->m_pUserDB->m_dwSerial;

  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  manager->Join(static_cast<unsigned int>(n), guildSerial, charSerial);
}

void CGuildBattleController::SendReservedScheduleList(
  int n,
  unsigned int uiMapID,
  unsigned int dwVer,
  unsigned __int8 byDay,
  unsigned __int8 byPage,
  unsigned int dwGuildSerial)
{
  GUILD_BATTLE::CGuildBattleReservedScheduleListManager *manager =
    GUILD_BATTLE::CGuildBattleReservedScheduleListManager::Instance();
  manager->Send(uiMapID, n, dwVer, byDay, byPage, dwGuildSerial);
}

void CGuildBattleController::SendCurrentBattleInfoRequest(int n, unsigned int uiMapID)
{
  GUILD_BATTLE::CCurrentGuildBattleInfoManager *manager = GUILD_BATTLE::CCurrentGuildBattleInfoManager::Instance();
  manager->Send(n, uiMapID);
}

void CGuildBattleController::CheckTakeGravityStone(int iPortalInx, CPlayer *pkPlayer)
{
  if (!pkPlayer)
  {
    return;
  }

  const int n = pkPlayer->m_ObjID.m_wIndex;
  unsigned int guildSerial = static_cast<unsigned int>(-1);
  if (pkPlayer->m_Param.m_pGuild)
  {
    guildSerial = pkPlayer->m_Param.m_pGuild->m_dwSerial;
  }
  const unsigned int charSerial = pkPlayer->m_pUserDB->m_dwSerial;

  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  manager->CheckTakeGravityStone(iPortalInx, n, guildSerial, charSerial);
}

void CGuildBattleController::CheckGetGravityStone(
  unsigned __int16 wIndex,
  unsigned int dwObjSerial,
  CPlayer *pkPlayer)
{
  if (!pkPlayer)
  {
    return;
  }

  const int n = pkPlayer->m_ObjID.m_wIndex;
  unsigned int guildSerial = static_cast<unsigned int>(-1);
  if (pkPlayer->m_Param.m_pGuild)
  {
    guildSerial = pkPlayer->m_Param.m_pGuild->m_dwSerial;
  }
  const unsigned int charSerial = pkPlayer->m_pUserDB->m_dwSerial;

  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  manager->CheckGetGravityStone(wIndex, dwObjSerial, n, guildSerial, charSerial);
}

void CGuildBattleController::CheckGoal(CPlayer *pkPlayer, int iPortalInx)
{
  if (!pkPlayer)
  {
    return;
  }

  const int n = pkPlayer->m_ObjID.m_wIndex;
  unsigned int guildSerial = static_cast<unsigned int>(-1);
  if (pkPlayer->m_Param.m_pGuild)
  {
    guildSerial = pkPlayer->m_Param.m_pGuild->m_dwSerial;
  }
  const unsigned int charSerial = pkPlayer->m_pUserDB->m_dwSerial;

  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  manager->CheckGoal(n, guildSerial, charSerial, iPortalInx);
}

bool CGuildBattleController::CheatCreateFieldObject(CPlayer *pkPlayer)
{
  GUILD_BATTLE::CNormalGuildBattleField *field = nullptr;
  if (pkPlayer->m_pCurMap)
  {
    CMapData *mapData = pkPlayer->m_pCurMap;
    const int raceCode = pkPlayer->m_Param.GetRaceCode();
    GUILD_BATTLE::CNormalGuildBattleFieldList *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();
    field = fieldList->GetField(raceCode, mapData->m_nMapCode);
  }
  return field && field->CreateFieldObject();
}

char CGuildBattleController::CheatDestroyFieldObject(CPlayer *pkPlayer)
{
  GUILD_BATTLE::CNormalGuildBattleField *field = nullptr;
  if (pkPlayer->m_pCurMap)
  {
    CMapData *mapData = pkPlayer->m_pCurMap;
    const int raceCode = pkPlayer->m_Param.GetRaceCode();
    GUILD_BATTLE::CNormalGuildBattleFieldList *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();
    field = fieldList->GetField(raceCode, mapData->m_nMapCode);
  }
  if (!field)
  {
    return 0;
  }
  field->DestroyFieldObject();
  return 1;
}

char CGuildBattleController::CheatDestroyStone(CPlayer *pkPlayer)
{
  GUILD_BATTLE::CNormalGuildBattleField *field = nullptr;
  if (pkPlayer->m_pCurMap)
  {
    CMapData *mapData = pkPlayer->m_pCurMap;
    const int raceCode = pkPlayer->m_Param.GetRaceCode();
    GUILD_BATTLE::CNormalGuildBattleFieldList *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();
    field = fieldList->GetField(raceCode, mapData->m_nMapCode);
  }
  if (!field)
  {
    return 0;
  }
  field->CheatDestroyStone();
  return 1;
}

bool CGuildBattleController::CheatDropStone(CPlayer *pkPlayer)
{
  GUILD_BATTLE::CNormalGuildBattleField *field = nullptr;
  if (pkPlayer->m_pCurMap)
  {
    CMapData *mapData = pkPlayer->m_pCurMap;
    const int raceCode = pkPlayer->m_Param.GetRaceCode();
    GUILD_BATTLE::CNormalGuildBattleFieldList *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();
    field = fieldList->GetField(raceCode, mapData->m_nMapCode);
  }
  return field && field->CheatDropStone(pkPlayer) == 0;
}

bool CGuildBattleController::CheatForceTakeStone(CPlayer *pkPlayer)
{
  GUILD_BATTLE::CNormalGuildBattleField *field = nullptr;
  if (pkPlayer->m_pCurMap)
  {
    CMapData *mapData = pkPlayer->m_pCurMap;
    const int raceCode = pkPlayer->m_Param.GetRaceCode();
    GUILD_BATTLE::CNormalGuildBattleFieldList *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();
    field = fieldList->GetField(raceCode, mapData->m_nMapCode);
  }
  return field && field->CheatForceTakeStone(pkPlayer);
}

void CGuildBattleController::Destroy()
{
  CGuildBattleController::Instance()->CleanUp();
}

void CGuildBattleController::DropGravityStone(CPlayer *pkPlayer)
{
  if (!pkPlayer)
  {
    return;
  }

  const int raceCode = pkPlayer->m_Param.GetRaceCode();
  (void)raceCode;

  unsigned int guildSerial = static_cast<unsigned int>(-1);
  if (pkPlayer->m_Param.m_pGuild)
  {
    guildSerial = pkPlayer->m_Param.m_pGuild->m_dwSerial;
  }
  const unsigned int charSerial = pkPlayer->m_pUserDB->m_dwSerial;

  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  const unsigned __int8 result = manager->DropGravityStone(guildSerial, charSerial);
  if (result)
  {
    GUILD_BATTLE::CGuildBattleLogger::Instance()->Log(
      "CGuildBattleController::DropGravityStone( %s ) : CNormalGuildBattleManager::Instance()->DropGravityStone( %u, %u ) Return(%u) Fail!",
      pkPlayer->m_Param.GetCharNameW(),
      guildSerial,
      charSerial,
      result);
  }
}

void CGuildBattleController::Kill(CPlayer *pkSrcPlayer, CPlayer *pkDestPlayer)
{
  const int raceCode = pkSrcPlayer->m_Param.GetRaceCode();
  (void)raceCode;

  unsigned int guildSerial = static_cast<unsigned int>(-1);
  if (pkSrcPlayer->m_Param.m_pGuild)
  {
    guildSerial = pkSrcPlayer->m_Param.m_pGuild->m_dwSerial;
  }
  const unsigned int srcCharSerial = pkSrcPlayer->m_pUserDB->m_dwSerial;
  const unsigned int destCharSerial = pkDestPlayer->m_pUserDB->m_dwSerial;

  GUILD_BATTLE::CNormalGuildBattleManager::Instance()->Kill(guildSerial, srcCharSerial, destCharSerial);
}

int CGuildBattleController::CheatRegenStone(CPlayer *pkPlayer, int iRengenPos)
{
  GUILD_BATTLE::CNormalGuildBattleField *field = nullptr;
  if (pkPlayer->m_pCurMap)
  {
    CMapData *curMap = pkPlayer->m_pCurMap;
    const int raceCode = pkPlayer->m_Param.GetRaceCode();
    GUILD_BATTLE::CNormalGuildBattleFieldList *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();
    field = fieldList->GetField(static_cast<unsigned __int8>(raceCode), curMap->m_nMapCode);
  }

  if (!field)
  {
    return -1;
  }

  if (iRengenPos < 0)
  {
    return field->CheatRegenStone(pkPlayer);
  }

  const int portalInx = field->CheatRegenStone(static_cast<unsigned int>(iRengenPos));
  if (portalInx < 0)
  {
    return -1;
  }

  _guild_battle_regen_stone_result_zocl msg{};
  msg.nPortalInx = portalInx;

  unsigned __int8 type[2]{};
  type[0] = 27;
  type[1] = 69;
  g_Network.m_pProcess[0]->LoadSendMsg(
    pkPlayer->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
  return portalInx;
}

char CGuildBattleController::CheatTakeStone(int iPortalInx, CPlayer *pkPlayer)
{
  GUILD_BATTLE::CNormalGuildBattleField *field = nullptr;
  if (pkPlayer->m_pCurMap)
  {
    CMapData *curMap = pkPlayer->m_pCurMap;
    const int raceCode = pkPlayer->m_Param.GetRaceCode();
    GUILD_BATTLE::CNormalGuildBattleFieldList *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();
    field = fieldList->GetField(static_cast<unsigned __int8>(raceCode), curMap->m_nMapCode);
  }

  if (!field)
  {
    return 0;
  }

  _guild_battle_get_gravity_stone_result_zocl msg{};
  msg.byRet = field->CheatTakeStone(iPortalInx, pkPlayer);
  if (!msg.byRet)
  {
    strcpy_0(msg.wszCharName, pkPlayer->m_Param.GetCharNameW());
    strcpy_0(msg.wszGuildName, "Cheat");
  }

  unsigned __int8 type[2]{};
  type[0] = 27;
  type[1] = 72;
  g_Network.m_pProcess[0]->LoadSendMsg(
    pkPlayer->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
  return 1;
}

char CGuildBattleController::CheatGetStone(CPlayer *pkPlayer)
{
  GUILD_BATTLE::CNormalGuildBattleField *field = nullptr;
  if (pkPlayer->m_pCurMap)
  {
    CMapData *curMap = pkPlayer->m_pCurMap;
    const int raceCode = pkPlayer->m_Param.GetRaceCode();
    GUILD_BATTLE::CNormalGuildBattleFieldList *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();
    field = fieldList->GetField(static_cast<unsigned __int8>(raceCode), curMap->m_nMapCode);
  }

  if (!field)
  {
    return 0;
  }

  _guild_battle_get_gravity_stone_result_zocl msg{};
  msg.byRet = field->CheatGetStone(pkPlayer);
  if (!msg.byRet)
  {
    strcpy_0(msg.wszCharName, pkPlayer->m_Param.GetCharNameW());
    strcpy_0(msg.wszGuildName, "Cheat");
  }

  unsigned __int8 type[2]{};
  type[0] = 27;
  type[1] = 72;
  g_Network.m_pProcess[0]->LoadSendMsg(
    pkPlayer->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
  return 1;
}

CGravityStoneRegener *CGuildBattleController::GetRegener(int iInx)
{
  GUILD_BATTLE::CNormalGuildBattleFieldList *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();
  return fieldList->GetRegener(iInx);
}

CCircleZone *CGuildBattleController::GetCircleZone(int iInx)
{
  GUILD_BATTLE::CNormalGuildBattleFieldList *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();
  return fieldList->GetCircleZone(iInx);
}

CGravityStone *CGuildBattleController::GetStone(int iInx)
{
  GUILD_BATTLE::CNormalGuildBattleFieldList *fieldList = GUILD_BATTLE::CNormalGuildBattleFieldList::Instance();
  return fieldList->GetStone(iInx);
}

void CGuildBattleController::SendPossibleBattleGuildListFirst(int n, unsigned __int8 byRace)
{
  GUILD_BATTLE::CPossibleBattleGuildListManager *manager = GUILD_BATTLE::CPossibleBattleGuildListManager::Instance();
  manager->SendFirst(n, byRace);
}
