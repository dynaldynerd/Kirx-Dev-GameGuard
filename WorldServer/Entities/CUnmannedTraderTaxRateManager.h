#pragma once

#include "IdaCompat.h"
#include "CMyTimer.h"
#include "TRC_AutoTrade.h"
#include <vector>

class CGuild;

class __cppobj CUnmannedTraderTaxRateManager
{
public:
  static CUnmannedTraderTaxRateManager *Instance();

  CUnmannedTraderTaxRateManager();
  bool Init(CLogFile *pkLogger);
  bool IsOwnerGuild(unsigned __int8 byRace, unsigned int dwGuildSerial);
  float GetTaxRate(unsigned __int8 byRace);
  void DQSCompleteInAtradTaxMoney(unsigned __int8 byRace, char *pdata);
  unsigned int GetTax(unsigned __int8 byRace, unsigned int dwGuildSerial, unsigned int dwPrice);
  int ChangeOwner(unsigned __int8 byRace, CGuild *pGuild);
  void SetPatriarchTaxMoney(unsigned __int8 byRace, unsigned int dwTax);

  static CUnmannedTraderTaxRateManager *ms_Instance;

  CMyTimer *m_pkTimer;
  std::vector<TRC_AutoTrade *> m_vecTRC;
};
