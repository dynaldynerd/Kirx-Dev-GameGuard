#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _holy_keeper_attackable_inform_zocl
{
  unsigned int dwKeeperSerial;
  unsigned __int8 byMasterRace;
  bool bAttackAbleState;

  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_holy_keeper_attackable_inform_zocl));
  }
};
#pragma pack(pop)
