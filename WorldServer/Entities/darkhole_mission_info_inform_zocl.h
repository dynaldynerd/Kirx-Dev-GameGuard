#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(1)) _darkhole_mission_info_inform_zocl
{
  struct __unaligned __declspec(align(1)) __job
  {
    unsigned __int8 byType;
    unsigned __int8 byTableCode;
    unsigned __int16 wRecordIndex;
    __int16 zNeedCount;
    char szDescirptCode[17];
    bool bPass;
    unsigned __int16 wCurCount;
    bool bDisable;
  };

  char szDescirptCode[17];
  unsigned int dwLimTimeSec;
  unsigned int dwPassTimeSec;
  unsigned __int8 byOrder;
  unsigned __int8 byJobNum;
  __job Job[128];

  _darkhole_mission_info_inform_zocl()
  {
    byJobNum = 0;
  }

  unsigned __int16 size();
};

inline unsigned __int16 _darkhole_mission_info_inform_zocl::size()
{
  if (byJobNum > 0x80u)
  {
    byJobNum = 0;
  }
  return static_cast<unsigned __int16>(3483 - 27 * (128 - byJobNum));
}
