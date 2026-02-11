#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _unit_part_tuning_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotIndex;
  unsigned __int8 byPart[6];
  unsigned int dwBullet[2];
  int nCost[7];
  unsigned int dwLeftMoney[7];
};
#pragma pack(pop)
