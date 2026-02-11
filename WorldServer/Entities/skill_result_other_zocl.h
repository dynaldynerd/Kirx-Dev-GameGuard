#pragma once

#include "IdaCompat.h"
#include "CHRID.h"

#pragma pack(push, 1)
struct _skill_result_other_zocl
{
  unsigned __int8 byRetCode;
  _CHRID idPerformer;
  _CHRID idDster;
  unsigned __int8 bySkillIndex;
  unsigned __int8 bySkillLv;
  unsigned __int8 byAttackSerial;
};
#pragma pack(pop)
