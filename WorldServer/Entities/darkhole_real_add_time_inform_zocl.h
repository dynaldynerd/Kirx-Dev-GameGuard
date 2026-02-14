#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _darkhole_real_add_time_inform_zocl
{
  int nAddSec;
  unsigned int dwTotalLimSec;
  char szMsg[17];

  unsigned __int16 size() const;
};
#pragma pack(pop)
