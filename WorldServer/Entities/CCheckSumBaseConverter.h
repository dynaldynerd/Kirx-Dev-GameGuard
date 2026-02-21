#pragma once

#include "IdaCompat.h"

class  CCheckSumBaseConverter
{
public:
  unsigned int ProcCode(unsigned __int8 byIndex, unsigned int dwSerial, unsigned int dwValue);
  long double ProcCode(unsigned __int8 byIndex, unsigned int dwSerial, long double dValue);

  static const unsigned int ms_dwKeys[6][10];
  static const long long ms_i64Keys[8][10];
};

