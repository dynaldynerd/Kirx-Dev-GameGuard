#pragma once

#include "IdaCompat.h"
#include "CGuild.h"
#include "CLogFile.h"
#include "CMyTimer.h"
#include "ControllerTaxRate.h"

#pragma pack(push, 1)
class  _suggested_matter_change_taxrate
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
#pragma pack(pop)

class  TRC_AutoTrade
{
public:
  TRC_AutoTrade();
  TRC_AutoTrade(unsigned char byRace);

  bool Initialzie();
  int check(unsigned int dwAvatorSerial, unsigned int dwGuildSerial);
  bool IsOwnerGuild(unsigned int nGuildSerial);
  bool IsMaster(unsigned int dwSerial);
  void sendmsg_taxrate(unsigned int n, unsigned __int8 byRet);
  void SendMsg_PatriarchTaxRate(unsigned int n);
  void SendMsg_UserLogInNotifyTaxRate(unsigned int n);
  float get_taxrate();
  float get_next_tax();
  unsigned __int8 get_race();
  char *get_guidlname();
  CGuild *getOwnerGuild();
  unsigned int getSuggestedTime();
  void set_suggested(
    unsigned __int8 byMatterType,
    unsigned int dwMatterDst,
    char *wszMatterDst,
    unsigned int dwNext);
  int ChangeTaxRate();
  int ChangeTaxRate(float fNewTaxRate);
  void PushDQSData();
  void PushDQSData_GuildInMoney(unsigned int dwRetPrice, unsigned int dwSeller);
  void his_income_money();
  void AddGDalant(char *pdata);
  unsigned int CalcPrice(unsigned int nGuildSerial, unsigned int nPrice);
  int ChangeOwner(CGuild *pGuild);
  void history_used_cheet_changetaxrate(unsigned int dwProb, char *pName);
  void SetGuildMaintainMoney(int dwTax, unsigned int dwSeller);
  void SetPatriarchTaxMoney(int dwTax);
  void set_owner(CGuild *pGuild);
  bool _db_load(unsigned __int8 byRace);
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
