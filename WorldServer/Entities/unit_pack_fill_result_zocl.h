#pragma once

#include "IdaCompat.h"
#include "unit_pack_fill_request_clzo.h"

#pragma pack(push, 1)
struct __cppobj __unaligned __declspec(align(1)) _unit_pack_fill_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotIndex;
  unsigned int dwComsumMoney[7];
  unsigned int dwLeftMoney[7];
  unsigned __int8 byFillNum;
  _unit_pack_fill_request_clzo::__list List[8];

  _unit_pack_fill_result_zocl() : byFillNum(0) {}
};
#pragma pack(pop)
