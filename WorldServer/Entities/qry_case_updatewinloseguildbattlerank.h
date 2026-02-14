#pragma once

#include "IdaCompat.h"

struct _qry_case_updatewinloseguildbattlerank
{
  unsigned __int8 byWinRace;
  unsigned int dwWinGuildSerial;
  unsigned __int8 byLoseRace;
  unsigned int dwLoseGuildSerial;

  __int64 size() const
  {
    return 16;
  }
};
