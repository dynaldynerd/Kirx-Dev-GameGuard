#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _mine_complete_result_zocl
{
  unsigned __int8 byErrCode;
  unsigned __int16 wEquipLeftDur;
  unsigned __int16 wBatteryLeftDur;
  unsigned __int8 byOreIndex;
  unsigned __int16 wOreSerial;
  unsigned __int8 byOreDur;
};
#pragma pack(pop)
