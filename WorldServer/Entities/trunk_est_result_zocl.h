#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _trunk_est_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned int dwLeftDalant;
  char szPW[16];

  _trunk_est_result_zocl();
};
#pragma pack(pop)

inline _trunk_est_result_zocl::_trunk_est_result_zocl()
{
  memset_0(this, 0, sizeof(*this));
}
