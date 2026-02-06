#include "pch.h"

#include "CGuildBattleController.h"
#include "CPlayer.h"
#include "CUserDB.h"
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

void CGuildBattleController::LogIn(CPlayer *pkPlayer)
{
  const int n = pkPlayer->m_ObjID.m_wIndex;
  const unsigned int dwGuildSerial = pkPlayer->m_Param.m_pGuild->m_dwSerial;
  const unsigned int dwCharacSerial = pkPlayer->m_pUserDB->m_dwSerial;
  GUILD_BATTLE::CNormalGuildBattleManager *manager = GUILD_BATTLE::CNormalGuildBattleManager::Instance();
  manager->LogIn(n, dwGuildSerial, dwCharacSerial);
}

void CGuildBattleController::SendPossibleBattleGuildListFirst(int n, unsigned __int8 byRace)
{
  GUILD_BATTLE::CPossibleBattleGuildListManager *manager = GUILD_BATTLE::CPossibleBattleGuildListManager::Instance();
  manager->SendFirst(n, byRace);
}
