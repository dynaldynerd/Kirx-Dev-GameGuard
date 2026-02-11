#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _a_trade_reg_item_request_clzo
{
  unsigned __int8 byStoreIndex;
  unsigned __int16 wItemSerial;
  unsigned __int8 byAmount;
  unsigned __int8 byItemTableCode;
  unsigned __int16 wItemIndex;
  unsigned int dwPrice;
  unsigned __int8 byTax;
  int bUseNpcLink;
};
#pragma pack(pop)
