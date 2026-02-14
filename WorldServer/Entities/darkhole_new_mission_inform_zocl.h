#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _darkhole_new_mission_inform_zocl
{
  struct __unaligned __declspec(align(1)) __job
  {
    unsigned __int8 byType;
    unsigned __int8 byTableCode;
    unsigned __int16 wRecordIndex;
    __int16 zNeedCount;
    char szDescirptCode[17];
    bool bDisable;
  };

  char szDescirptCode[17];
  unsigned int dwLimTimeSec;
  unsigned __int8 byOrder;
  unsigned __int8 byJobNum;
  __job Job[128];

  _darkhole_new_mission_inform_zocl()
    : szDescirptCode{},
      dwLimTimeSec(0),
      byOrder(0),
      byJobNum(0),
      Job{}
  {
  }

  unsigned __int16 size();
};
#pragma pack(pop)

inline unsigned __int16 _darkhole_new_mission_inform_zocl::size()
{
  if (byJobNum > 128u)
  {
    byJobNum = 0;
  }

  constexpr unsigned __int16 kHeaderSize = 23;
  constexpr unsigned __int16 kJobSize = 24;
  return static_cast<unsigned __int16>(kHeaderSize + kJobSize * byJobNum);
}
