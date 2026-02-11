#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _make_tower_result_zocl
{
  unsigned __int8 byErrCode;
  unsigned int dwTowerObjSerial;
  unsigned __int16 wLeftFP;
};
#pragma pack(pop)
