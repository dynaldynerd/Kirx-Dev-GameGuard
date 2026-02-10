#pragma once

#include "IdaCompat.h"

struct _pvppoint_guild_rank_info_list
{
  unsigned int dwSerial;
  unsigned __int16 wRank;
  char wszGuildName[17];
  unsigned __int8 byRace;
  unsigned __int8 byGrade;
  long double dKillPvpPoint;
  long double dGuildBattlePvpPoint;
  long double dSumPvpPoint;
};

struct _pvppoint_guild_rank_info
{
  unsigned __int16 wRaceCnt[4];
  unsigned __int16 wCount;
  _pvppoint_guild_rank_info_list list[500];
};
