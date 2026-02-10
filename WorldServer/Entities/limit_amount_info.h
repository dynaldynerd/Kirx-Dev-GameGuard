#pragma once

#include "IdaCompat.h"

struct __declspec(align(4)) _limit_amount_info
{
  struct __declspec(align(4)) _item_info
  {
    unsigned int dwLimitItemIndex;
    unsigned __int16 wLimitNum;
  };

  unsigned __int8 byItemNum;
  _item_info ItemInfo[16];

  _limit_amount_info()
  {
    memset_0(this, 0, sizeof(_limit_amount_info));
  }
};
