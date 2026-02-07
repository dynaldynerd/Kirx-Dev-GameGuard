#pragma once

#include "IdaCompat.h"

#include <vector>

class CPlayer;
class INationGameGuardSystem;
class CBilling;
class CashDbWorker;
struct _NameTxt_fld;
struct _CashShop_str_fld;

struct CHEAT_COMMAND
{
  const char *pwszCommand;
  unsigned int uiCmdLen;
  bool (__fastcall *fn)(CPlayer *);
  int nUseDegree;
  int nMgrDegree;
};

class __cppobj CNationSettingData
{
public:
  enum CASH_DB_PROC_FLAG : int
  {
    CDPS_USE_REF = 0,
    CDPS_SET = 1,
    CDPS_INIT = 2,
    CDPS_NOT_REF = -1,
  };

  virtual ~CNationSettingData() = default;
  virtual CashDbWorker *CreateWorker();
  virtual CBilling *CreateBilling();
  virtual const char *GetItemName(struct _NameTxt_fld *pFld);
  virtual bool IsApplyPcbangPrimium(const CPlayer *pUser);
  virtual int GetCashItemPrice(_CashShop_str_fld *pFld);
  const char *GetNoneString();
  bool IsCashDBUseExtRef();
  bool IsCashDBInit();
  bool IsCashDBDSNSetted();
  void SetCashDBInitFlag();

  bool m_bServiceMode;
  int m_iNationCode;
  char m_szNationCodeStr[3];
  int m_iANSICodePage;
  char m_szNoneString[8];
  char m_szCashDBName[64];
  char m_szCashDBIP[16];
  char m_szCashDBID[64];
  char m_szCashDBPW[64];
  unsigned __int16 m_wCashDBPort;
  CASH_DB_PROC_FLAG m_eCashDBFlag;
  char m_szWorldDBID[64];
  char m_szWorldDBPW[64];
  unsigned __int16 m_wBillingForceCloseDelay;
  char m_szVaildKey[17];
  std::vector<CHEAT_COMMAND> m_vecCheatData;
  INationGameGuardSystem *m_pGameGuardSystem;
};
