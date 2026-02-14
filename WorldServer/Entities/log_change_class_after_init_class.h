#pragma once

#include "IdaCompat.h"

struct __declspec(align(4)) _log_change_class_after_init_class
{
  unsigned int dwCharacSerial;
  unsigned __int8 byType;
  char szPrevClassCode[5];
  char szNextClassCode[5];
  int nClassInitCnt;
  unsigned __int8 byLastClassGrade;
  unsigned __int16 dwYear;
  unsigned __int8 byMonth;
  unsigned __int8 byDay;
  unsigned __int8 byHour;
  unsigned __int8 byMin;
  unsigned __int8 bySec;

  __int64 size() const
  {
    return 32;
  }
};
