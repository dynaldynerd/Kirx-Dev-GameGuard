#include "pch.h"

#include "economy_history_data.h"

_economy_history_data::_economy_history_data()
{
  Init();
}

void _economy_history_data::Init()
{
  for (int j = 0; j < 3; ++j)
  {
    dTradeGold[j] = DOUBLE_1_0;
    dTradeDalant[j] = DOUBLE_1_0;
    wEconomyGuide[j] = 100;
    for (int k = 0; k < 3; ++k)
    {
      dOreMineCount[j][k] = DOUBLE_1_0;
      dOreCutCount[j][k] = DOUBLE_1_0;
    }
  }
}
