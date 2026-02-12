#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(2)) _darkhole_member_info_inform_zocl
{
  struct __unaligned __declspec(align(1)) __list
  {
    char wszName[17];
    unsigned int dwSerial;
  };

  unsigned int dwLeaderSerial;
  unsigned __int16 wMemberNum;
  __list List[32];

  _darkhole_member_info_inform_zocl()
  {
    wMemberNum = 0;
  }

  unsigned __int16 size();
};

inline unsigned __int16 _darkhole_member_info_inform_zocl::size()
{
  if (wMemberNum > 0x20u)
  {
    wMemberNum = 32;
  }
  return static_cast<unsigned __int16>(678 - 21 * (32 - wMemberNum));
}
