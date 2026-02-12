#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _trunk_potionsocket_division_result_zocl
{
  char sErrorCode;
  unsigned __int16 wSerial;
  unsigned __int16 wParentAmount;
  unsigned __int16 wChildSerial;
  unsigned __int16 wChildAmount;

  _trunk_potionsocket_division_result_zocl();
};
#pragma pack(pop)

inline _trunk_potionsocket_division_result_zocl::_trunk_potionsocket_division_result_zocl()
{
  memset_0(this, 0, sizeof(*this));
}
