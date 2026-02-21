#pragma once
#include "IdaCompat.h"
#include "attack_gen_result_zocl.h"
#pragma pack(push, 1)
struct _attack_unit_result_zocl
{
  unsigned int dwAtterSerial;
  unsigned __int8 byWeaponPart;
  unsigned __int16 wWeaponIndex;
  unsigned __int16 wBulletIndex;
  unsigned __int8 byAttackPart;
  bool bCritical;
  unsigned __int8 byListNum;
  _attack_gen_result_zocl::_dam_list DamList[32];

  _attack_unit_result_zocl() : byListNum(0)
  {
  }

  int size()
  {
    if (byListNum > 0x20u)
    {
      byListNum = 0;
    }
    return 332 - 10 * (32 - byListNum);
  }
};
#pragma pack(pop)

static_assert(sizeof(_attack_unit_result_zocl) == 332, "_attack_unit_result_zocl packing mismatch");
