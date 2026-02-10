#pragma once

#include "IdaCompat.h"
#include "CGuild.h"
#include "CLogFile.h"
#include "CMyTimer.h"
#include "ControllerTaxRate.h"

class __cppobj _suggested_matter_change_taxrate
{
public:
  _suggested_matter_change_taxrate();
  void init();

  unsigned char byMatterType;
  unsigned int dwMatterDst;
  char wszMatterDst[64];
  unsigned int dwNext;
  unsigned int dwSuggestedTime;
};

class __cppobj TRC_AutoTrade
{
public:
  TRC_AutoTrade();
  TRC_AutoTrade(unsigned char byRace);

  bool Initialzie();
  bool IsOwnerGuild(unsigned int nGuildSerial);
  float get_taxrate();
  void AddGDalant(char *pdata);
  unsigned int CalcPrice(unsigned int nGuildSerial, unsigned int nPrice);
  int ChangeOwner(CGuild *pGuild);
  void SetPatriarchTaxMoney(int dwTax);
  static unsigned __int8 _insert_info(char *pdata);

  bool m_bInit;
  unsigned char m_byCurDay;
  unsigned short m_wCurMonth;
  unsigned short m_wCurYear;
  bool m_bChangeTaxRate;
  float m_fCommonTaxRate;
  CGuild *m_pOwnerGuild;
  ControllerTaxRate m_Controller;
  CLogFile m_sysLog;
  CLogFile m_serviceLog;
  unsigned int m_dwTrade;
  long double m_dIncomeMoney;
  unsigned char m_byRace;
  _suggested_matter_change_taxrate m_suggested;
};
