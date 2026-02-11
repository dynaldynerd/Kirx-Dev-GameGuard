#pragma once

#include "IdaCompat.h"

struct _economy_calc_data
{
  long double dTradeGold[3];
  long double dTradeDalant[3];
  long double dOreMineCount[3][3];
  long double dOreCutCount[3][3];
  float out_fPayExgRate[3];
  float out_fTexRate[3];
  float out_fOreRate[3];
  unsigned __int16 out_wEconomyGuide[3];
  unsigned int out_dwTexRate[3];
};
