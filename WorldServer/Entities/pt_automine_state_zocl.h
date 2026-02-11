#pragma once

#include "IdaCompat.h"
#include "InvenKey.h"

struct __cppobj __unaligned __declspec(align(2)) _pt_automine_state_zocl
{
  unsigned __int8 bySelectOre;
  unsigned int dwGage;
  unsigned __int8 byPage;
  _INVENKEY item;

  _pt_automine_state_zocl();
  __int64 size() const;
};

inline _pt_automine_state_zocl::_pt_automine_state_zocl()
{
  item = _INVENKEY();
}

inline __int64 _pt_automine_state_zocl::size() const
{
  return 10;
}
