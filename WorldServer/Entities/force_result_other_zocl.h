#pragma once

#include "IdaCompat.h"
#include "CHRID.h"

#pragma pack(push, 1)
struct _force_result_other_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 byForceIndex;
  unsigned __int8 byForceLv;
  _CHRID idPerformer;
  _CHRID idDster;
  unsigned __int8 byAttackSerial;
};
#pragma pack(pop)
