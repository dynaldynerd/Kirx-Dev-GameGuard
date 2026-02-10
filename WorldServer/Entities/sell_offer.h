#pragma once

#include "IdaCompat.h"
#include "StorageList.h"

struct __declspec(align(8)) _sell_offer
{
  _STORAGE_LIST::_db_con *pItem;
  unsigned __int8 byAmount;
  unsigned __int8 bySlotIndex;
  unsigned __int8 byStorageCode;
};
