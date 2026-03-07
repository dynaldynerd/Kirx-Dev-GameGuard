#pragma once

#include "IdaCompat.h"

struct _weeklyguildrank_owner_info
{
  struct _list
  {
    unsigned int dwSerial;
    char wszGuildName[17];
    unsigned __int8 byRace;
    unsigned __int16 wRank;
    unsigned __int8 byGrade;
    long double dKillPvpPoint;
    long double dGuildBattlePvpPoint;
    unsigned int dwSumLv;
    unsigned int dwSumRankScore;
  };

  unsigned __int16 wRaceCnt[4];
  unsigned __int16 wCount;
  _list list[6];
};
