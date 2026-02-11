#pragma once

#include "IdaCompat.h"
#include "CHRID.h"

#pragma pack(push, 1)
struct _class_skill_result_other_zocl
{
  unsigned __int8 byRetCode;
  _CHRID idPerformer;
  _CHRID idDster;
  unsigned __int16 wSkillIndex;
  unsigned __int8 byAttackSerial;
};
#pragma pack(pop)
