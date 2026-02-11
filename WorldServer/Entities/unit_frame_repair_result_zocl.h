#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(2)) _unit_frame_repair_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotIndex;
  unsigned int dwNewGauge;
  unsigned int dwConsumDalant;
  unsigned int dwLeftDalant;
};
#pragma pack(pop)
