#pragma once

#include "IdaCompat.h"

struct _worlddb_guild_battle_info
{
  struct __list
  {
    unsigned int dwID;
    unsigned int dwP1GuildSerial;
    unsigned int dwP2GuildSerial;
    unsigned int dwMapID;
    unsigned __int8 byNumber;
  };

  unsigned __int16 wCount;
  __list list[69];
};

