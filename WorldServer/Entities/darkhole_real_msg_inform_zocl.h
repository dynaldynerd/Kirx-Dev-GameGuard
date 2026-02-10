#pragma once

#include "IdaCompat.h"

struct _darkhole_real_msg_inform_zocl
{
  char szMsg[17];

  unsigned __int16 size() const;
};
