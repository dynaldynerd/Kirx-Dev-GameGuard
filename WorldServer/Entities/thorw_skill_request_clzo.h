#pragma once

#include "IdaCompat.h"
#include "CHRID.h"

#pragma pack(push, 1)
struct _thorw_skill_request_clzo
{
  unsigned __int16 wBulletSerial;
  _CHRID idDst;
  unsigned __int8 byAttackSerial;
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)
