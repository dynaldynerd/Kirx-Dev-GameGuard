#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(1)) _personal_amine_fixpos_zocl
{
  unsigned int dwObjSerial;
  unsigned __int16 wObjIndex;
  float fFixPos[3];
  unsigned __int8 byAct;
  unsigned __int16 wItemTblIndex;
  unsigned __int16 wItemSerial;
  unsigned int dwOwnerSeiral;

  _personal_amine_fixpos_zocl();
  unsigned __int16 size() const;
};

inline _personal_amine_fixpos_zocl::_personal_amine_fixpos_zocl()
{
  memset_0(this, 0, sizeof(_personal_amine_fixpos_zocl));
}

inline unsigned __int16 _personal_amine_fixpos_zocl::size() const
{
  return 25;
}
