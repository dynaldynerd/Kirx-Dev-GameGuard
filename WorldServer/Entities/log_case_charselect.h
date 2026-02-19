#pragma once

#include "IdaCompat.h"

struct _log_case_charselect
{
  char szID[13];
  unsigned int dwIDSerial;
  char wszName[17];
  unsigned int dwNameSerial;
  unsigned __int16 dwYear;
  unsigned __int8 byMonth;
  unsigned __int8 byDay;
  unsigned __int8 byHour;
  unsigned __int8 byMin;
  unsigned __int8 bySec;

  __int64 size() const
  {
    return 52;
  }
};

