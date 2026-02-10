#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _limit_item_num_info_zocl
{
  struct _limit_item_info
  {
    unsigned int dwLimitItemIndex;
    unsigned __int16 wLimitNum;
  };

  unsigned int dwStoreIndex;
  unsigned __int8 byLimitItemNum;
  _limit_item_info LimitItemInfo[16];

  _limit_item_num_info_zocl()
  {
    memset_0(this, 0, sizeof(_limit_item_num_info_zocl));
  }

  unsigned __int16 size()
  {
    if (byLimitItemNum > 0x10u)
    {
      byLimitItemNum = 0;
    }
    return static_cast<unsigned __int16>(101 - 6LL * (16 - byLimitItemNum));
  }
};
#pragma pack(pop)
