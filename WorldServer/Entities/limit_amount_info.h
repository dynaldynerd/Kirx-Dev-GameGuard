#pragma once

#include "IdaCompat.h"

struct _limit_amount_info
{
  struct _item_info
  {
    unsigned int dwLimitItemIndex;
    unsigned __int16 wLimitNum;
  };

  unsigned __int8 byItemNum;
  _item_info ItemInfo[16];

  _limit_amount_info()
  {
    std::memset(this, 0, sizeof(_limit_amount_info));
  }
};

