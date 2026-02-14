#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __cppobj __unaligned __declspec(align(1)) _personal_automine_stop_zocl
{
  unsigned int dwObjSerial;
  unsigned int dwOwnerSerial;
  unsigned __int8 byStopType;
  unsigned __int16 wItemSerial;

  _personal_automine_stop_zocl();
  __int64 size() const;
};
#pragma pack(pop)

inline _personal_automine_stop_zocl::_personal_automine_stop_zocl()
{
  memset_0(this, 0, sizeof(_personal_automine_stop_zocl));
}

inline __int64 _personal_automine_stop_zocl::size() const
{
  return 11;
}
