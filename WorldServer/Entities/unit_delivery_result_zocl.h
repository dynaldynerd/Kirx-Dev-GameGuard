#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _unit_delivery_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotIndex;
  unsigned __int8 byTransDistCode;
  unsigned int dwParkingUnitSerial;
  unsigned int dwPayDalant;
  unsigned int dwLeftDalant;
};
#pragma pack(pop)
