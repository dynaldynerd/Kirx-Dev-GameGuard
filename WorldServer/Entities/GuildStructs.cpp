#include "pch.h"

#include "CMainThread.h"

_guild_member_info::_guild_member_info()
{
  init();
}

void _guild_member_info::init()
{
  dwSerial = static_cast<unsigned int>(-1);
  wszName[0] = 0;
  byClassInGuild = static_cast<unsigned __int8>(-1);
  byLv = 0;
  dwPvpPoint = 0;
  byRank = 0;
  byGrade = 0;
  pPlayer = nullptr;
  bVote = false;
}

bool _guild_member_info::IsFill()
{
  return dwSerial != static_cast<unsigned int>(-1);
}

_guild_applier_info::_guild_applier_info()
{
  init();
}

void _guild_applier_info::init()
{
  pPlayer = nullptr;
  dwApplyTime = 0;
}

bool _guild_applier_info::IsFill()
{
  return pPlayer != nullptr;
}

_guild_battle_suggest_matter::_guild_battle_suggest_matter()
{
  Clear();
}

void _guild_battle_suggest_matter::Clear()
{
  eState = WAIT_BATTLE_REQUEST;
  pkSrc = nullptr;
  pkDest = nullptr;
  dwStartTime = 0;
  dwNumber = 0;
  dwMapIdx = 0;
}
