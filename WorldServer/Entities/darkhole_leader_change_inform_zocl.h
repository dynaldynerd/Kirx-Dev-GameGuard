#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _darkhole_leader_change_inform_zocl
{
  unsigned int dwNewLeaderSerial;

  unsigned __int16 size() const;
};
#pragma pack(pop)
