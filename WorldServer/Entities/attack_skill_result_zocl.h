#pragma once

#include "IdaCompat.h"
#include "attack_gen_result_zocl.h"

#pragma pack(push, 1)
struct _attack_skill_result_zocl
{
  unsigned __int8 byAtterID;
  unsigned int dwAtterSerial;
  unsigned __int8 byEffectCode;
  unsigned __int16 wSkillIndex;
  unsigned __int8 bySkillLv;
  unsigned __int8 byAttackPart;
  unsigned __int16 wBulletIndex;
  bool bCritical;
  __int16 zAttackPos[2];
  bool bWPActive;
  unsigned __int8 byListNum;
  _attack_gen_result_zocl::_dam_list DamList[32];

  _attack_skill_result_zocl() : byListNum(0) {}

  int size()
  {
    if (byListNum > 0x20u)
    {
      byListNum = 0;
    }
    return 339 - 10 * (32 - byListNum);
  }
};
#pragma pack(pop)
