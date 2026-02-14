#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __cppobj _guild_alter_member_state_inform_zocl
{
  struct __unaligned __declspec(align(1)) __list
  {
    unsigned int dwMemberSerial;
    unsigned __int8 byLv;
    unsigned int dwPvpPoint;
  };

  unsigned __int8 byAlterMemberNum;
  __list MemberList[50];

  _guild_alter_member_state_inform_zocl();
  __int64 size();
};
#pragma pack(pop)
