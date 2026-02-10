#pragma once

#include "IdaCompat.h"
#include "CPlayer.h"

#pragma pack(push, 1)
struct __cppobj _unit_download_result_zocl
{
  struct __cppobj __unaligned __declspec(align(1)) _list
  {
    unsigned __int8 bySlotIndex;
    _UNIT_DB_BASE::_LIST UnitData;

    _list();
  };

  unsigned __int8 byUnitNum;
  _list UnitList[4];

  _unit_download_result_zocl();
  unsigned __int64 size();
};
#pragma pack(pop)
