#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _make_tower_request_clzo
{
  struct __material
  {
    unsigned __int16 wItemSerial;
    unsigned __int8 byMaterSlotIndex;
    unsigned __int8 byAmount;
  };

  unsigned __int16 wSkillIndex;
  __int16 zPos[3];
  unsigned __int16 wTowerItemSerial;
  unsigned __int16 wConsumeItemSerial[3];
  unsigned __int8 byMaterialNum;
  __material Material[30];
};
#pragma pack(pop)
