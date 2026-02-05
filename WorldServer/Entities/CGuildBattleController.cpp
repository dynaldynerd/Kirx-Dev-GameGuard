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
  return true;
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
