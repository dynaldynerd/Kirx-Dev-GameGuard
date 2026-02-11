#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _make_trap_request_clzo
{
  unsigned __int16 wSkillIndex;
  __int16 zPos[3];
  unsigned __int16 wTrapItemSerial;
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)
