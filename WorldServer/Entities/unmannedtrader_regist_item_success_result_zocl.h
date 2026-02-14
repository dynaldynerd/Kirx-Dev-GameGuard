#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _unmannedtrader_regist_item_success_result_zocl
{
  unsigned __int16 wItemSerial;
  unsigned int dwPrice;
  unsigned int dwRegedSerial;
  unsigned int dwListIndex;
  unsigned int dwTax;
  unsigned int dwLeftDalant;
  unsigned __int16 wSepaSerial;
  unsigned __int8 bySepaAmount;

  __int64 size() const;
};
#pragma pack(pop)

inline __int64 _unmannedtrader_regist_item_success_result_zocl::size() const
{
  return 25;
}
