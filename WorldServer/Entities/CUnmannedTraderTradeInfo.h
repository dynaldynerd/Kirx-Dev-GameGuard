#pragma once

#include "IdaCompat.h"

class CMyTimer;

class __cppobj CUnmannedTraderTradeInfo
{
public:
  CMyTimer *m_pkTimer;
  int m_iOldDay;
  bool m_bNeedUpdateSave;
  unsigned long long m_ui64TotalOldIncome;
  unsigned long long m_ui64TotalCurrentIncome;
};
