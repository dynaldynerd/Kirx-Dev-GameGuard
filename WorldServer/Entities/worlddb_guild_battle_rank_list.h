#pragma once

#include "IdaCompat.h"

struct _worlddb_guild_battle_rank_list
{
  struct __list
  {
    int nRank;
    unsigned __int8 byGrade;
    char wszName[17];
    unsigned int dwWin;
    unsigned int dwLose;
    unsigned int dwDraw;
    unsigned int dwScore;
    unsigned int dwSerial;
  };

  unsigned __int16 wCount;
  __list list[500];
};
