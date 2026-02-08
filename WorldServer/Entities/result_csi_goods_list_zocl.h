#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _result_csi_goods_list_zocl
{
  int nCashAmount;
  bool bAdjustDiscountRate;
  bool bOneNOne;
  bool bSetDiscount;
  unsigned __int8 bySetDiscount[4];
  bool bLimSale;
  unsigned __int8 byLimDiscount;

  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_result_csi_goods_list_zocl));
  }
};
#pragma pack(pop)
