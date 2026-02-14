#pragma once

#include "IdaCompat.h"

struct _qry_case_updatedrawguildbattlerank
{
  unsigned __int8 by1PRace;
  unsigned int dw1PGuildSerial;
  unsigned __int8 by2PRace;
  unsigned int dw2PGuildSerial;

  __int64 size() const
  {
    return 16;
  }
};
