#pragma once

#include "IdaCompat.h"

#include "CTSingleton.h"

class CBilling;
class CPlayer;
class CashDbWorker;

class CNationSettingData;
struct _NameTxt_fld;
struct _CashShop_str_fld;

class __cppobj CNationSettingManager : public CTSingleton<CNationSettingManager>
{
public:
  int Init(int nationCode, const char *nationCodeStr, bool serviceMode);
  CBilling *CreateBilling();
  CashDbWorker *CreateWorker();
  int GetCashItemPrice(_CashShop_str_fld *pFld);
  const char *GetItemName(_NameTxt_fld *pFld);
  const char *GetNoneString();
  bool IsApplyPcbangPrimium(const CPlayer *pUser);
  bool IsCashDBUseExtRef();
  bool IsCashDBInit();
  bool IsCashDBDSNSetted();
  unsigned __int16 GetBillingForceCloseDelay();
  char *GetCashDBName();
  char *GetCashDBDBIP();
  char *GetCashDBID();
  char *GetCashDBPW();
  unsigned __int16 GetCashDBPort();
  char *GetNationCodeStr();
  void SetCashDBInitState();

  CNationSettingData *m_pData;
};
