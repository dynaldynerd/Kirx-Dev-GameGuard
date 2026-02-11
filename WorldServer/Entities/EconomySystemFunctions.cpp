#include "pch.h"

#include "EconomySystemFunctions.h"

#include "ECONOMY_SYSTEM.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

#include <cmath>

char _ReadEconomyIniFile()
{
  e_nOreDft = GetPrivateProfileIntA("Economy", "Default_OreVal", 0, ".\\Initialize\\WorldSystem.ini");
  if (e_nOreDft)
  {
    return 1;
  }

  MyMessageBox("Economy Error", "Nothing Default MgrValue");
  return 0;
}

int _CalcPayExgRatePerRace(float *pfAvrTradeMoney, int nRaceCode)
{
  const float v5 = pfAvrTradeMoney[nRaceCode];
  const float v6 = pfAvrTradeMoney[(nRaceCode + 1) % 3];
  const float v7 = pfAvrTradeMoney[(nRaceCode + 2) % 3];
  int result = 0;
  if ((v5 + v6 + v7) != 0.0f)
  {
    result = static_cast<int>(((v6 + v7) * 0.5f / (v5 + v6 + v7)) * 3000.0f + static_cast<float>(e_nMgrDft));
  }
  if (result < 1)
  {
    return 1;
  }
  return result;
}

void _UpdateNewEconomy(_economy_calc_data *pData)
{
  float pfAvrTradeMoney[3]{};
  for (int raceCode = 0; raceCode < 3; ++raceCode)
  {
    const double sum = static_cast<double>(pData->dTradeGold[raceCode])
                     + static_cast<double>(pData->dTradeDalant[raceCode]) / 1000.0
                     + 1.0;
    pfAvrTradeMoney[raceCode] = static_cast<float>(std::sqrt(sum));
    if (pfAvrTradeMoney[raceCode] < 1.0f)
    {
      pfAvrTradeMoney[raceCode] = FLOAT_1_0;
    }
  }

  float tradeMoney[3]{};
  for (int raceCode = 0; raceCode < 3; ++raceCode)
  {
    const int rate = _CalcPayExgRatePerRace(pfAvrTradeMoney, raceCode);
    pData->out_fPayExgRate[raceCode] = static_cast<float>(rate);
    tradeMoney[raceCode] =
      static_cast<float>(pData->dTradeGold[raceCode]
                         + pData->dTradeDalant[raceCode] / pData->out_fPayExgRate[raceCode]);
    if (tradeMoney[raceCode] < 1.0f)
    {
      tradeMoney[raceCode] = FLOAT_1_0;
    }
  }

  float avgTrade = (tradeMoney[0] + tradeMoney[1] + tradeMoney[2]) / 3.0f;
  if (avgTrade < 1.0f)
  {
    avgTrade = FLOAT_1_0;
  }

  for (int raceCode = 0; raceCode < 3; ++raceCode)
  {
    pData->out_wEconomyGuide[raceCode] =
      static_cast<unsigned __int16>(static_cast<int>((tradeMoney[raceCode] / avgTrade) * 100.0f + 0.5f));
    pData->out_fTexRate[raceCode] =
      static_cast<float>((tradeMoney[raceCode] / avgTrade) * static_cast<float>(e_nTaxDft) / 100.0f);
    pData->out_dwTexRate[raceCode] = static_cast<unsigned int>(pData->out_fTexRate[raceCode] * 10000.0f);
  }

  float oreRate[3]{};
  for (int raceCode = 0; raceCode < 3; ++raceCode)
  {
    double cutSum = pData->dOreCutCount[raceCode][0] * 1.5
                  + pData->dOreCutCount[raceCode][1] * 2.0
                  + pData->dOreCutCount[raceCode][2] * 2.5;
    if (cutSum <= 0.0)
    {
      cutSum = DOUBLE_1_0;
    }
    double mineSum = pData->dOreMineCount[raceCode][0] * 1.5
                   + pData->dOreMineCount[raceCode][1] * 2.0
                   + pData->dOreMineCount[raceCode][2] * 2.5;
    if (mineSum <= 0.0)
    {
      mineSum = DOUBLE_1_0;
    }
    oreRate[raceCode] = static_cast<float>(cutSum / mineSum) + static_cast<float>(e_nOreDft / 10000.0);
    if (oreRate[raceCode] > 1.0f)
    {
      oreRate[raceCode] = FLOAT_1_0;
    }
  }

  float avgOre = (oreRate[0] + oreRate[1] + oreRate[2]) / 3.0f;
  if (avgOre <= 0.0f)
  {
    for (int raceCode = 0; raceCode < 3; ++raceCode)
    {
      pData->out_fOreRate[raceCode] = FLOAT_1_0;
    }
  }
  else
  {
    for (int raceCode = 0; raceCode < 3; ++raceCode)
    {
      pData->out_fOreRate[raceCode] = (oreRate[raceCode] / avgOre) * 0.25f + oreRate[raceCode] * 0.75f;
      if (pData->out_fOreRate[raceCode] > 1.0f)
      {
        pData->out_fOreRate[raceCode] = FLOAT_1_0;
      }
    }
  }
}

char eInitEconomySystem(
  int nCurMgrValue,
  int nNextMgrValue,
  _economy_history_data *pData,
  int nHisNum,
  _economy_history_data *pCurData)
{
  if (e_EconomySystem.m_bLoad)
  {
    return 1;
  }

  if (nHisNum < 12)
  {
    MyMessageBox(
      "Economy Error",
      "number of history(%d) is more than MAX_ECONOMY_HISTORY (%d)",
      nHisNum,
      12);
    return 0;
  }

  e_EconomySystem.Init();
  e_EconomySystem.m_dwLastDate = eGetLocalDate();
  if (!_ReadEconomyIniFile())
  {
    return 0;
  }

  for (int j = 0; j < 3; ++j)
  {
    e_EconomyHistory[j].Init();
  }

  for (int k = 0; k < nHisNum; ++k)
  {
    _economy_calc_data calc{};
    for (int j = 0; j < 3; ++j)
    {
      calc.dTradeDalant[j] = pData[k].dTradeDalant[j];
      calc.dTradeGold[j] = DOUBLE_1_0;
      for (int m = 0; m < 3; ++m)
      {
        calc.dOreMineCount[j][m] = pData[k].dOreMineCount[j][m];
        calc.dOreCutCount[j][m] = pData[k].dOreCutCount[j][m];
      }
    }
    _UpdateNewEconomy(&calc);
    for (int j = 0; j < 3; ++j)
    {
      e_EconomyHistory[k].dTradeDalant[j] = calc.dTradeDalant[j];
      e_EconomyHistory[k].dTradeGold[j] = calc.dTradeGold[j];
      e_EconomyHistory[k].wEconomyGuide[j] = calc.out_wEconomyGuide[j];
      for (int n = 0; n < 3; ++n)
      {
        e_EconomyHistory[k].dOreMineCount[j][n] = calc.dOreMineCount[j][n];
        e_EconomyHistory[k].dOreCutCount[j][n] = calc.dOreCutCount[j][n];
      }
    }
  }

  e_EconomySystem.m_byCurHour = static_cast<unsigned __int8>(GetCurrentHour());

  _economy_calc_data now{};
  for (int j = 0; j < 3; ++j)
  {
    now.dTradeDalant[j] = pData[11].dTradeDalant[j];
    now.dTradeGold[j] = DOUBLE_1_0;
    for (int ii = 0; ii < 3; ++ii)
    {
      now.dOreMineCount[j][ii] = pData[11].dOreMineCount[j][ii];
      now.dOreCutCount[j][ii] = pData[11].dOreCutCount[j][ii];
    }
  }

  _UpdateNewEconomy(&now);
  for (int j = 0; j < 3; ++j)
  {
    e_EconomySystem.m_CurRate[j].dOldTradeDalant = now.dTradeDalant[j];
    e_EconomySystem.m_CurRate[j].dOldTradeGold = now.dTradeGold[j];
    e_EconomySystem.m_CurRate[j].fPayExgRate = now.out_fPayExgRate[j];
    e_EconomySystem.m_CurRate[j].fTexRate = now.out_fTexRate[j];
    e_EconomySystem.m_CurRate[j].fOreRate = now.out_fOreRate[j];
    e_EconomySystem.m_CurRate[j].wEconomyGuide = now.out_wEconomyGuide[j];
    e_EconomySystem.m_CurRate[j].dwTexRate = now.out_dwTexRate[j];
    for (int jj = 0; jj < 3; ++jj)
    {
      e_EconomySystem.m_CurRate[j].dOldOreMineCount[jj] = now.dOreMineCount[j][jj];
      e_EconomySystem.m_CurRate[j].dOldOreCutCount[jj] = now.dOreCutCount[j][jj];
    }

    e_EconomySystem.m_dCurTradeDalant[j] = pCurData->dTradeDalant[j];
    e_EconomySystem.m_dCurTradeGold[j] = pCurData->dTradeGold[j];
    e_EconomySystem.m_dBufTradeGold[j] = 0.0;
    e_EconomySystem.m_dBufTradeDalant[j] = 0.0;
    for (int jj = 0; jj < 3; ++jj)
    {
      e_EconomySystem.m_dCurOreMineCount[j][jj] = pCurData->dOreMineCount[j][jj];
      e_EconomySystem.m_dCurOreCutCount[j][jj] = pCurData->dOreCutCount[j][jj];
      e_EconomySystem.m_dBufOreMineCount[j][jj] = 0.0;
      e_EconomySystem.m_dBufOreCutCount[j][jj] = 0.0;
    }
  }

  e_EconomySystem.m_bLoad = true;
  (void)nCurMgrValue;
  (void)nNextMgrValue;
  return 1;
}
