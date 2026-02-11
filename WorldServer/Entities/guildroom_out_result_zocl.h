#pragma once

#include "IdaCompat.h"

struct _guildroom_out_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 byMapIndex;
  unsigned __int16 wMapLayerIndex;
  __int16 sNewPos[3];

  __int64 size();
};
