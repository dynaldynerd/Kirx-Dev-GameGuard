#pragma once

#include "IdaCompat.h"

struct _ECONOMY_SYSTEM
{
  struct _CUR_RATE
  {
    float fPayExgRate;
    float fTexRate;
    float fOreRate;
    unsigned __int16 wEconomyGuide;
    long double dOldTradeGold;
    long double dOldTradeDalant;
    long double dOldOreMineCount[3];
    long double dOldOreCutCount[3];
    unsigned int dwTexRate;
  };

  bool m_bLoad;
  long double m_dCurTradeGold[3];
  long double m_dCurTradeDalant[3];
  long double m_dBufTradeGold[3];
  long double m_dBufTradeDalant[3];
  long double m_dCurOreMineCount[3][3];
  long double m_dCurOreCutCount[3][3];
  long double m_dBufOreMineCount[3][3];
  long double m_dBufOreCutCount[3][3];
  _CUR_RATE m_CurRate[3];
  unsigned __int8 m_byCurHour;
  unsigned int m_dwLastUpdateTime;
  unsigned int m_dwSystemOperStartTime;
  unsigned int m_dwLastDate;

  _ECONOMY_SYSTEM();
  void Init();
  void CurTradeMoneyInit();
};

