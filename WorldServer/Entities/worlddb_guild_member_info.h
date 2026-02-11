#pragma once

#include "IdaCompat.h"

struct __declspec(align(4)) _worlddb_guild_member_info
{
  struct __declspec(align(4)) __guild_member_info
  {
    unsigned int dwSerial;
    char wszName[17];
    unsigned __int8 byClassInGuild;
    unsigned __int8 byLv;
    unsigned int dwPvpPoint;
    unsigned __int16 wRank;
  };

  unsigned __int16 wMemberCount;
  __guild_member_info MemberData[50];
};
