#include "pch.h"

#include "CGuildBattleController.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "GuildBattle.h"

CGuildBattleController *CGuildBattleController::Instance()
{
  static CGuildBattleController s_instance;
  return &s_instance;
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
  CRFNewDatabase::SetAutoCommitMode(g_Main.m_pWorldDB, 0);
  if (g_Main.m_pWorldDB->UpdateGuildBattleInfo(
        *reinterpret_cast<unsigned int *>(szData),
        *reinterpret_cast<unsigned int *>(szData + 4),
        *reinterpret_cast<unsigned int *>(szData + 8),
        *reinterpret_cast<unsigned int *>(szData + 12),
        static_cast<unsigned __int8>(szData[16]))
      && g_Main.m_pWorldDB->UpdateGuildBattleScheduleInfo(
        *reinterpret_cast<unsigned int *>(szData),
        *reinterpret_cast<unsigned int *>(szData + 20),
        static_cast<unsigned __int8>(szData[24]),
        *reinterpret_cast<unsigned __int64 *>(szData + 32),
        *reinterpret_cast<unsigned __int16 *>(szData + 40)))
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
  return rankManager->SelectGuildBattleRankList(byRace, byOutData);
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
  return reserved->UpdateReservedShedule(dwSLID, byOutData);
}

void CGuildBattleController::CompleteUpdateReservedSchedule(unsigned int dwMapID, unsigned __int8 *pLoadData)
{
  GUILD_BATTLE::CGuildBattleReservedScheduleListManager *reserved =
    GUILD_BATTLE::CGuildBattleReservedScheduleListManager::Instance();
  reserved->UpdateTomorrowComplete(dwMapID, pLoadData);
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

void CGuildBattleController::SendPossibleBattleGuildListFirst(int n, unsigned __int8 byRace)
{
  GUILD_BATTLE::CPossibleBattleGuildListManager *manager = GUILD_BATTLE::CPossibleBattleGuildListManager::Instance();
  manager->SendFirst(n, byRace);
}
