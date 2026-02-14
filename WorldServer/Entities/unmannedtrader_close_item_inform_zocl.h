#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _unmannedtrader_close_item_inform_zocl
{
  unsigned __int8 byTaxRate;
  unsigned __int16 wItemSerial;
  unsigned int dwRegistSerial;
  unsigned int dwPrice;

  __int64 size() const;
};
#pragma pack(pop)

inline __int64 _unmannedtrader_close_item_inform_zocl::size() const
{
  return 11;
}
