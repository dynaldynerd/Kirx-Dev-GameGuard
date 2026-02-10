#pragma once

#include "IdaCompat.h"

class __cppobj CCheckSumBaseConverter
{
public:
  unsigned int ProcCode(unsigned __int8 byIndex, unsigned int dwSerial, unsigned int dwValue);
  long double ProcCode(unsigned __int8 byIndex, unsigned int dwSerial, long double dValue);

  static const unsigned int *ms_dwKeys[8];
  static const long long *ms_i64Keys[8];
};

