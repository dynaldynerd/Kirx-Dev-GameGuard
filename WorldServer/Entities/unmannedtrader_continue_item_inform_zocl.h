#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(2)) _unmannedtrader_continue_item_inform_zocl
{
  struct __list
  {
    unsigned __int16 wItemSerial;
    unsigned int dwRegistSerial;
    unsigned int dwPrice;
  };

  unsigned __int8 byTaxRate;
  unsigned __int8 byNum;
  __list List[10];

  __int64 size() const;
};

inline __int64 _unmannedtrader_continue_item_inform_zocl::size() const
{
  if (byNum > 0xAu)
  {
    const_cast<_unmannedtrader_continue_item_inform_zocl *>(this)->byNum = 0;
  }
  return 102 - 10LL * (10 - byNum);
}
