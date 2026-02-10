#pragma once

#include "IdaCompat.h"
#include "attack_gen_result_zocl.h"

#pragma pack(push, 1)
struct _attack_siege_result_zocl
{
  unsigned __int8 byAtterID;
  unsigned int dwAtterSerial;
  unsigned __int8 byAttackPart;
  unsigned __int16 wBulletIndex;
  bool bCritical;
  __int16 zTar[2];
  unsigned __int8 byListNum;
  _attack_gen_result_zocl::_dam_list DamList[32];

  _attack_siege_result_zocl() : byListNum(0) {}

  int size()
  {
    if (byListNum > 0x20u)
    {
      byListNum = 0;
    }
    return 334 - 10 * (32 - byListNum);
  }
};
#pragma pack(pop)
