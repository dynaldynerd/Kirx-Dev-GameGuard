#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _skill_recall_teleport_request_clzo
{
  unsigned __int16 wSkillIndex;
  char wszRecallName[17];
  unsigned __int16 wConsumeItemSerial[3];
};
#pragma pack(pop)
