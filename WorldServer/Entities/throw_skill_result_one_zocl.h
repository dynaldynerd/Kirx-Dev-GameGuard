#pragma once

#include "IdaCompat.h"
#include "CHRID.h"

#pragma pack(push, 1)
struct _throw_skill_result_one_zocl
{
  struct __effected_list
  {
    unsigned __int8 byObjID;
    unsigned int dwObjSerial;
    unsigned __int8 byRetCode;
    unsigned __int16 wEffectValue;
  };

  unsigned __int8 byErrCode;
  unsigned __int8 byAttackSerial;
  _CHRID idDster;
  unsigned __int8 byEffectedNum;
  __effected_list list[30];

  _throw_skill_result_one_zocl() : byEffectedNum(0) {}
};
#pragma pack(pop)
