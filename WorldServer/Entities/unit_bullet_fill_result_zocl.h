#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(2)) _unit_bullet_fill_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotIndex;
  unsigned __int16 wBulletIndex[2];
  unsigned int dwComsumMoney[7];
  unsigned int dwLeftMoney[7];
};
#pragma pack(pop)
