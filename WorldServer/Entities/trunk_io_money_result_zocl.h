#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _trunk_io_money_result_zocl
{
  unsigned __int8 byRetCode;
  long double dTrunkDalant;
  long double dTrunkGold;
  unsigned int dwCharDalant;
  unsigned int dwCharGold;
  unsigned int dwFeeDalant;

  _trunk_io_money_result_zocl();
};
#pragma pack(pop)

inline _trunk_io_money_result_zocl::_trunk_io_money_result_zocl()
{
  memset_0(this, 0, sizeof(*this));
}
