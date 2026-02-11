#pragma once

#include "economy_calc_data.h"
#include "economy_history_data.h"

char eInitEconomySystem(
  int nCurMgrValue,
  int nNextMgrValue,
  _economy_history_data *pData,
  int nHisNum,
  _economy_history_data *pCurData);
char _ReadEconomyIniFile();
void _UpdateNewEconomy(_economy_calc_data *pData);
int _CalcPayExgRatePerRace(float *pfAvrTradeMoney, int nRaceCode);
