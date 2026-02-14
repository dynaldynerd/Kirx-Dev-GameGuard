#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _guildroom_out_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 byMapIndex;
  unsigned __int16 wMapLayerIndex;
  __int16 sNewPos[3];

  __int64 size();
};
#pragma pack(pop)
