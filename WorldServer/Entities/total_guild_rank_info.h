#pragma once

#include "IdaCompat.h"

struct _total_guild_rank_info_list
{
  unsigned __int16 wRank;
  unsigned int dwSerial;
  long double dPowerPoint;
  char wszGuildName[17];
  unsigned __int8 byRace;
  unsigned __int8 byGrade;
  unsigned int dwMasterSerial;
  char wszMasterName[17];
};

struct _total_guild_rank_info
{
  unsigned __int16 wRaceCnt[4];
  unsigned __int16 wCount;
  _total_guild_rank_info_list list[500];
};

