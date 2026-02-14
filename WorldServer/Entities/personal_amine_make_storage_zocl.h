#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __cppobj __unaligned __declspec(align(1)) _personal_amine_make_storage_zocl
{
  unsigned int dwAvatorSerial;
  unsigned int dwTotGold;

  _personal_amine_make_storage_zocl();
  __int64 size() const;
};
#pragma pack(pop)

inline _personal_amine_make_storage_zocl::_personal_amine_make_storage_zocl()
{
  dwTotGold = 0;
}

inline __int64 _personal_amine_make_storage_zocl::size() const
{
  return 8;
}
