#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _unit_return_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned int dwPayDalant;
  unsigned int dwNewDalant;
};
#pragma pack(pop)
