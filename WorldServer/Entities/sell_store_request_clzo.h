#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _sell_store_request_clzo
{
  struct _list
  {
    unsigned __int8 byStorageCode;
    unsigned __int16 wSerial;
    unsigned __int8 byAmount;
  };

  unsigned int dwStoreIndex;
  unsigned __int8 bySellNum;
  int bUseNPCLinkIntem;
  _list Item[100];
};
#pragma pack(pop)
