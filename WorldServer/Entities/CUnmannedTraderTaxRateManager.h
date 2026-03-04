#pragma once

#include "IdaCompat.h"
#include "CMyTimer.h"
#include "TRC_AutoTrade.h"
#include <vector>

class CGuild;

class  CUnmannedTraderTaxRateManager
{
public:
  static CUnmannedTraderTaxRateManager *Instance();
  static void Destroy();

  CUnmannedTraderTaxRateManager();
  ~CUnmannedTraderTaxRateManager();
  bool Init(CLogFile *pkLogger);
  void CleanUp();
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
  char CheatChangeTaxRate(unsigned __int8 byRace, int dwNewTaxRate, char *pCheaterName);
  void SetGuildMaintainMoney(unsigned __int8 byRace, unsigned int dwTax, unsigned int dwSeller);
  int ChangeOwner(unsigned __int8 byRace, CGuild *pGuild);
  void SetPatriarchTaxMoney(unsigned __int8 byRace, unsigned int dwTax);

  static CUnmannedTraderTaxRateManager *ms_Instance;

  CMyTimer *m_pkTimer;
  std::vector<TRC_AutoTrade *> m_vecTRC;
};
