#pragma once

#include "IdaCompat.h"

struct _darkhole_leader_change_inform_zocl
{
  unsigned int dwNewLeaderSerial;

  unsigned __int16 size() const;
};
