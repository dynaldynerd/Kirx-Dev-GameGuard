#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _darkhole_job_count_inform_zocl
{
  unsigned __int8 byJobIndex;
  unsigned __int16 wJobCount;

  unsigned __int16 size() const;
};
#pragma pack(pop)
