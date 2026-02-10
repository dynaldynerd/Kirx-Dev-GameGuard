#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _sell_store_result_zocl
{
  unsigned __int8 bSucc;
  unsigned int dwLeftDalant;
  unsigned int dwLeftGold;
  unsigned int dwProfitDanlant;
  unsigned int dwProfitGold;
  unsigned __int8 byDiscountRate;
  unsigned __int8 byRetCode;
};
#pragma pack(pop)
