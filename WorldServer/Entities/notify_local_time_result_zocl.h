#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _notify_local_time_result_zocl
{
  unsigned __int8 byRet;
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;

  __int64 size() const;
};
#pragma pack(pop)

inline __int64 _notify_local_time_result_zocl::size() const
{
  return byRet ? 1LL : 37LL;
}
