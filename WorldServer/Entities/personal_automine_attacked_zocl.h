#pragma once

#include "IdaCompat.h"

struct __cppobj _personal_automine_attacked_zocl
{
  unsigned __int16 wItemSerial;

  _personal_automine_attacked_zocl();
  unsigned __int16 size() const;
};

inline _personal_automine_attacked_zocl::_personal_automine_attacked_zocl()
{
  memset_0(this, 0, sizeof(_personal_automine_attacked_zocl));
}

inline unsigned __int16 _personal_automine_attacked_zocl::size() const
{
  return 2;
}
