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
  void Loop();
  char Load();
  void CompleteCreate(unsigned __int16 wInx);
  bool IsOwnerGuild(unsigned __int8 byRace, unsigned int dwGuildSerial);
  float GetTaxRate(unsigned __int8 byRace);
  void SendTaxRate(int n, unsigned __int8 byRace);
  void SendTaxRatePatriarch(int n, unsigned __int8 byRace);
  unsigned int GetSuggestedTime(unsigned __int8 byRace);
  void SetSuggested(
    unsigned __int8 byRace,
    unsigned __int8 byMatterType,
    unsigned int dwMatterDst,
    char *wszMatterDst,
    unsigned int dwNext);
  void DQSCompleteInAtradTaxMoney(unsigned __int8 byRace, char *pdata);
  unsigned int GetTax(unsigned __int8 byRace, unsigned int dwGuildSerial, unsigned int dwPrice);
  int ChangeOwner(unsigned __int8 byRace, CGuild *pGuild);
  void SetPatriarchTaxMoney(unsigned __int8 byRace, unsigned int dwTax);

  static CUnmannedTraderTaxRateManager *ms_Instance;

  CMyTimer *m_pkTimer;
  std::vector<TRC_AutoTrade *> m_vecTRC;
};
