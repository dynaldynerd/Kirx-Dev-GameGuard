#pragma once

#include "IdaCompat.h"

struct _worlddb_item_list
{
  struct __item_charge_info
  {
    unsigned int uiSerial;
    unsigned int dwItemCode_K;
    unsigned __int64 dwItemCode_D;
    unsigned int dwItemCode_U;
    unsigned int dwT;
    unsigned __int64 lnUID;
    char szDate[32];
  };

  unsigned __int8 byItemCount;
  __item_charge_info itemList[32];
};
