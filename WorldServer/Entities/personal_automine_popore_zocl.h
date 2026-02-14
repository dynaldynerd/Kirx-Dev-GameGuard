#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __cppobj __unaligned __declspec(align(1)) _personal_automine_popore_zocl
{
  unsigned __int16 wItemSerial;
  unsigned __int8 byNum;

  _personal_automine_popore_zocl();
  __int64 size() const;
};
#pragma pack(pop)

inline _personal_automine_popore_zocl::_personal_automine_popore_zocl()
{
  memset_0(this, 0, sizeof(_personal_automine_popore_zocl));
}

inline __int64 _personal_automine_popore_zocl::size() const
{
  return 3;
}
