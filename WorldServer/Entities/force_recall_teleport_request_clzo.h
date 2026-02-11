#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _force_recall_teleport_request_clzo
{
  unsigned __int16 wForceSerial;
  char wszRecallName[17];
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)
