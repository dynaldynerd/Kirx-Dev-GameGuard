#pragma once

#include "IdaCompat.h"

struct  _log_sheet_lv
{
  unsigned int dwAvatorSerial;
  unsigned __int8 byLv;
  unsigned int dwTotalPlayMin;

  __int64 size();
};
