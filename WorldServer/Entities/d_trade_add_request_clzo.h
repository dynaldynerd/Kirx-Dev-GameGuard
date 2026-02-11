#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _d_trade_add_request_clzo
{
  unsigned __int8 byStorageCode;
  unsigned int dwSerial;
  unsigned __int8 bySlotIndex;
  unsigned __int8 byAmount;
};
#pragma pack(pop)
