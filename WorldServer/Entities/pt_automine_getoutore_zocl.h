#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(1)) _pt_automine_getoutore_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wItemSerial;

  _pt_automine_getoutore_zocl();
  __int64 size() const;
};

inline _pt_automine_getoutore_zocl::_pt_automine_getoutore_zocl()
{
  memset_0(this, 0, sizeof(_pt_automine_getoutore_zocl));
}

inline __int64 _pt_automine_getoutore_zocl::size() const
{
  return 3;
}
