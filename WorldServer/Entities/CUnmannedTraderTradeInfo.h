#pragma once

#include "IdaCompat.h"

class CMyTimer;

class __cppobj CUnmannedTraderTradeInfo
{
public:
  CUnmannedTraderTradeInfo();
  ~CUnmannedTraderTradeInfo();

  char Init();
  void Loop();
  void LoadINI();
  void SaveINI();
  void UpdateIncome();
  void NotifyIncome();
  void NotifyIncome(unsigned __int8 byRace, unsigned __int16 wIndex);
  void AddIncome(unsigned int dwOriPrice);

  CMyTimer *m_pkTimer;
  int m_iOldDay;
  bool m_bNeedUpdateSave;
  unsigned long long m_ui64TotalOldIncome;
  unsigned long long m_ui64TotalCurrentIncome;
};
