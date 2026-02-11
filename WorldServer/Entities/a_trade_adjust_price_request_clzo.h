#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _a_trade_adjust_price_request_clzo
{
  unsigned __int8 byStoreIndex;
  unsigned __int16 wItemSerial;
  unsigned int dwNewPrice;
  unsigned int dwRegistSerial;
  unsigned __int8 byTaxRate;
};
#pragma pack(pop)
