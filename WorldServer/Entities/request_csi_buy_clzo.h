#pragma once

#include "IdaCompat.h"
#include "StoragePos.h"

#pragma pack(push, 1)
struct _request_csi_buy_clzo
{
  struct __item
  {
    int nPrice;
    unsigned __int8 byDiscount;
    unsigned __int8 byTblCode;
    unsigned __int16 wItemIdx;
    unsigned __int16 wStoreIdx;
    unsigned __int8 byOverlapNum;
    int nEvent[8];
    unsigned __int8 byEventType;
  };

  unsigned __int8 nNum;
  unsigned __int8 bySetKind;
  unsigned __int8 byCouponNum;
  _STORAGE_POS_INDIV CouponItem[3];
  __item item[20];
};
#pragma pack(pop)
