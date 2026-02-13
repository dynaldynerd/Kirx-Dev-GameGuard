#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(2)) _personal_automine_alter_dur_zocl
{
  unsigned int dwObjSerial;
  unsigned __int16 wHPRate;

  _personal_automine_alter_dur_zocl();
  unsigned __int16 size() const;
};

inline _personal_automine_alter_dur_zocl::_personal_automine_alter_dur_zocl()
{
  memset_0(this, 0, sizeof(_personal_automine_alter_dur_zocl));
}

inline unsigned __int16 _personal_automine_alter_dur_zocl::size() const
{
  return 6;
}
