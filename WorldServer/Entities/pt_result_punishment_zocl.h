#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __declspec(align(1)) _pt_result_punishment_zocl
{
  unsigned __int8 byType;
  char wszCharName[17];

  __int64 size();
};
#pragma pack(pop)
