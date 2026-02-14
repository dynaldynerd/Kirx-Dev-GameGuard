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

_suggested_matter::_suggested_matter()
{
  Clear();
}

void _suggested_matter::Clear()
{
  byMatterType = static_cast<unsigned __int8>(-1);
  dwMatterDst = 0;
  wszMatterDst[0] = 0;
  dwMatterObj1 = 0;
  dwMatterObj2 = 0;
  dwMatterObj3 = 0;
  byVoteState[0] = 0;
  byVoteState[1] = 0;
  wszComment[0] = 0;
  dwMatterVoteSynKey = 0;
  dwStartTime = 0;
  nTotal_VotableMemNum = 0;

  for (int index = 0; index < 50; ++index)
  {
    VotableMem[index] = nullptr;
  }
}

bool _suggested_matter::IsVotable(unsigned int dwSerial)
{
  for (int index = 0; index < nTotal_VotableMemNum; ++index)
  {
    if (VotableMem[index]->dwSerial == dwSerial)
    {
      return true;
    }
  }

  return false;
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
