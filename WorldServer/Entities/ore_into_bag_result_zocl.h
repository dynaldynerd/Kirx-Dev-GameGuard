#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _ore_into_bag_result_zocl
{
  unsigned __int8 byErrCode;
  unsigned __int16 wNewSerial;
  unsigned __int8 byCsMethod;
  unsigned int dwT;
};
#pragma pack(pop)
