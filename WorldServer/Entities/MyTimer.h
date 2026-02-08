#pragma once

#include "IdaCompat.h"

class MyTimer
{
public:
  struct TIME
  {
    int _nYear;
    int _nMonth;
    int _nDay;
    int _nHour;
    int _nMin;
    int _nSec;

    TIME();
  };

  static const TIME *GetTime();

private:
  static TIME _time;
};
