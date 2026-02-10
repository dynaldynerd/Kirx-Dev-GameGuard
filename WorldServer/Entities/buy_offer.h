#pragma once

#include "IdaCompat.h"
#include "StorageList.h"

struct _buy_offer
{
  unsigned __int8 byGoodIndex;
  unsigned __int8 byGoodAmount;
  unsigned __int8 byStorageCode;
  _STORAGE_LIST::_db_con Item;
  unsigned __int16 wSerial;
};
