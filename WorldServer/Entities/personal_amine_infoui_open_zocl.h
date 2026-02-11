#pragma once

#include "IdaCompat.h"

struct __cppobj _personal_amine_infoui_open_zocl
{
  unsigned int dwObjSerial;
  unsigned int dwBattery[2];

  _personal_amine_infoui_open_zocl();
  __int64 size() const;
};

inline _personal_amine_infoui_open_zocl::_personal_amine_infoui_open_zocl()
{
  memset_0(this, 0, sizeof(_personal_amine_infoui_open_zocl));
}

inline __int64 _personal_amine_infoui_open_zocl::size() const
{
  return 12;
}
