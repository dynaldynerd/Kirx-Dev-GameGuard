#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _unit_sell_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotIndex;
  unsigned __int16 wKeySerial;
  unsigned int dwNonPayDalant;
  int nAddMoney[7];
  unsigned int dwLeftMoney[7];
};
#pragma pack(pop)
