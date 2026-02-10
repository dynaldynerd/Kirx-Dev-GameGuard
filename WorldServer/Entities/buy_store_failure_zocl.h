#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _buy_store_failure_zocl
{
  unsigned int dwDalant;
  unsigned int dwGold;
  long double dPoint;
  unsigned int dwActPoint[3];
  unsigned __int8 byRetCode;
};
#pragma pack(pop)
