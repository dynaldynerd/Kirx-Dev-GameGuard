#pragma once

#include "IdaCompat.h"
#include "CMyTimer.h"
#include "TRC_AutoTrade.h"
#include <vector>

class __cppobj CUnmannedTraderTaxRateManager
{
public:
  static CUnmannedTraderTaxRateManager *Instance();

  CUnmannedTraderTaxRateManager();
  bool Init(CLogFile *pkLogger);
  bool IsOwnerGuild(unsigned __int8 byRace, unsigned int dwGuildSerial);
  float GetTaxRate(unsigned __int8 byRace);

  static CUnmannedTraderTaxRateManager *ms_Instance;

  CMyTimer *m_pkTimer;
  std::vector<TRC_AutoTrade *> m_vecTRC;
};
