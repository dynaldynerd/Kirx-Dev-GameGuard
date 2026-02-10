#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _buy_store_success_zocl
{
  struct _list
  {
    unsigned __int16 wSerial;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
  };

  unsigned int dwLeftDalant;
  unsigned int dwLeftGold;
  unsigned int dwConsumDanlant;
  unsigned int dwConsumGold;
  unsigned int dwLeftPoint;
  unsigned int dwConsumPoint;
  unsigned int dwLeftActPoint[3];
  unsigned int dwConsumActPoint[3];
  unsigned __int8 byDiscountRate;
  unsigned __int8 byBuyNum;
  _list OfferList[100];

  _buy_store_success_zocl() : byBuyNum(0) {}

  unsigned __int16 size()
  {
    if (byBuyNum > 0x64u)
    {
      byBuyNum = 0;
    }
    return static_cast<unsigned __int16>(750 - 7LL * (100 - byBuyNum));
  }
};
#pragma pack(pop)
