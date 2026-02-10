#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _buy_store_request_clzo
{
  struct _list
  {
    unsigned __int8 byStorageCode;
    unsigned int dwGoodSerial;
    unsigned __int8 byAmount;
  };

  unsigned int dwStoreIndex;
  unsigned __int8 byBuyNum;
  int bUseNPCLinkIntem;
  _list OfferList[100];
};
#pragma pack(pop)
