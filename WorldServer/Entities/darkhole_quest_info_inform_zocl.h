#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(1)) _darkhole_quest_info_inform_zocl
{
  char szDescirptCode[17];
  unsigned int dwLimTimeSec;
  unsigned int dwPassTimeSec;
  long double dRwExp;
  unsigned int dwRwPvp;
  unsigned int dwRwDalant;

  unsigned __int16 size() const;
};

inline unsigned __int16 _darkhole_quest_info_inform_zocl::size() const
{
  return 41;
}
