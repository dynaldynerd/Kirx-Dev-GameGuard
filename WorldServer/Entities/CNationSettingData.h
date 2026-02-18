#pragma once

#include "IdaCompat.h"

#include <vector>

#include "CHEAT_COMMAND.h"

class CPlayer;
class INationGameGuardSystem;
class CBilling;
class CashDbWorker;
struct _NameTxt_fld;
struct _CashShop_str_fld;

class  CNationSettingData
{
public:
  enum CASH_DB_PROC_FLAG : int
  {
    CDPS_USE_REF = 0,
    CDPS_SET = 1,
    CDPS_INIT = 2,
    CDPS_NOT_REF = -1,
  };

  CNationSettingData();
  ~CNationSettingData() = default;

  virtual int Init();
  virtual void Loop();
  virtual CashDbWorker *CreateWorker();
  virtual CBilling *CreateBilling();
  virtual const char *GetItemName(_NameTxt_fld *pFld);
  virtual bool ValidMacAddress();
  virtual bool IsNormalString(const wchar_t *wszString);
  virtual bool IsNormalString(const char *szString);
  virtual bool IsNormalChar(const wchar_t wcChar);
  virtual void SetUnitPassiveValue(float *fUnitPv_DefFc);
  virtual bool IsPersonalFreeFixedAmountBillingType(__int16 *pDest1, __int16 *pDest2);
  virtual bool CheckEnterWorldRequest(int n, char *pBuf);
  virtual void CreateComplete(CPlayer *pOne);
  virtual void NetClose(CPlayer *pOne);
  virtual int GetCashItemPrice(_CashShop_str_fld *pFld);
  virtual bool IsApplyPcbangPrimium(const CPlayer *pUser);
  virtual void SendCashDBDSNRequest();
  virtual bool ReadSystemPass();

  char *GetNoneString();
  bool IsCashDBUseExtRef();
  bool IsCashDBInit();
  bool IsCashDBDSNSetted();
  INationGameGuardSystem *GetGameGuardSystem();
  void SetCashDBInitFlag();
  void SetCashDBDSN(char *szIP, char *szDBName, char *szAccount, char *szPassword, unsigned __int16 dwPort);
  void SetCashDBDSNSetFlag();
  void SetCahsDBUseExtRefFlag();
  bool CheckDBCSCompleteString(unsigned int nCodePage, const char *strData, unsigned __int64 *pCharacterCount);
  bool IsNormalStringDefProc(const char *szString, const char *szException);
  bool IsNormalStringDefProc(const wchar_t *wszString, const wchar_t *wszException);
  bool GetFireGuardEnableSetting();
  bool GetTimeLimitEnableSetting();

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
