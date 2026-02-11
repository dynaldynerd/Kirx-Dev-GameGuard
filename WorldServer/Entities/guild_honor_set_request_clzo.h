#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _guild_honor_set_request_clzo
{
  struct __list
  {
    char wszGuildName[17];
    unsigned __int8 byTaxRate;
  };

  unsigned __int8 byListNum;
  __list GuildList[5];
};
#pragma pack(pop)
