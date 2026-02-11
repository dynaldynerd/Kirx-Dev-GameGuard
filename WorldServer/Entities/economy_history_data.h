#pragma once

#include "IdaCompat.h"

struct __cppobj _economy_history_data
{
  long double dTradeGold[3];
  long double dTradeDalant[3];
  unsigned __int16 wEconomyGuide[3];
  long double dOreMineCount[3][3];
  long double dOreCutCount[3][3];

  _economy_history_data();
  void Init();
};
