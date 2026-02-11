#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(2)) _unit_frame_buy_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 byFrameCode;
  unsigned __int8 byAddUnitSlot;
  unsigned __int8 byKeyIndex;
  unsigned __int16 wKeySerial;
  unsigned int dwLeftMoney[7];
  unsigned int dwConsumMoney[7];
};
#pragma pack(pop)
