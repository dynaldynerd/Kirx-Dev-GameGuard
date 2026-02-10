#pragma once

#include "IdaCompat.h"

struct _worlddb_guild_battle_reserved_schedule_info
{
  struct __declspec(align(4)) __list
  {
    unsigned int dw1PGuildSerial;
    char wsz1PName[17];
    unsigned __int8 by1PRace;
    unsigned int dw2PGuildSerial;
    char wsz2PName[17];
    unsigned __int8 by2PRace;
    unsigned __int8 byStartHour;
    unsigned __int8 byStartMin;
    unsigned __int8 byEndHour;
    unsigned __int8 byEndMin;
  };

  unsigned __int16 wCount;
  __list list[46];
};
