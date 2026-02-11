#pragma once

#include "IdaCompat.h"

struct _worlddb_guild_battle_schedule_list
{
  struct __declspec(align(8)) __list
  {
    unsigned int dwID;
    unsigned int dwSLID;
    unsigned __int8 ucState;
    __int64 tStartTime;
    unsigned __int16 wTumeMin;
    unsigned __int8 byNumber;
  };

  unsigned __int16 wCount;
  __list list[23];
};
