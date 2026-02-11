#pragma once

#include "IdaCompat.h"
#include "CHRID.h"
#include "throw_skill_result_one_zocl.h"

#pragma pack(push, 1)
struct _throw_unit_result_other_zocl
{
  unsigned __int8 byRetCode;
  _CHRID idPerformer;
  unsigned __int16 wBulletIndex;
  unsigned __int8 byAttackSerial;
  _CHRID idDster;
  unsigned __int8 byEffectedNum;
  _throw_skill_result_one_zocl::__effected_list list[30];

  _throw_unit_result_other_zocl() : byEffectedNum(0) {}
};
#pragma pack(pop)
