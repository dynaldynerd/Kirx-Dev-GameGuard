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
void eUpdateEconomySystem(bool *pbChangeDay);
void SendMsg_EconomyDataToWeb();
void _UpdateRateSendToAllPlayer();
void OnLoop_VoteSystem();
void OnLoop_GuildSystem(bool bChangeDay);
