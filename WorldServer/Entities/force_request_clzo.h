#pragma once

#include "IdaCompat.h"
#include "CHRID.h"

#pragma pack(push, 1)
struct _force_request_clzo
{
  unsigned __int16 wForceSerial;
  _CHRID idDst;
  unsigned __int8 byAttackSerial;
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)
