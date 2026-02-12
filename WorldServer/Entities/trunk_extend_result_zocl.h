#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(2)) _trunk_extend_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 byPackNum;
  unsigned int dwLeftDalant;
  unsigned int dwConsumDanlant;

  _trunk_extend_result_zocl();
};
#pragma pack(pop)

inline _trunk_extend_result_zocl::_trunk_extend_result_zocl()
{
  memset_0(this, 0, sizeof(*this));
}
