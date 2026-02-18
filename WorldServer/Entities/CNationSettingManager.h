#pragma once

#include "IdaCompat.h"

#include "CTSingleton.h"

class CBilling;
class CPlayer;
class CashDbWorker;
class CHEAT_COMMAND;

class CNationSettingData;
struct _NameTxt_fld;
struct _CashShop_str_fld;
struct _MSG_HEADER;

class  CNationSettingManager : public CTSingleton<CNationSettingManager>
{
public:
  CNationSettingManager();
  ~CNationSettingManager();

  int Init(int nationCode, const char *nationCodeStr, bool serviceMode);
  CBilling *CreateBilling();
  CashDbWorker *CreateWorker();
  int GetCashItemPrice(_CashShop_str_fld *pFld);
  const char *GetItemName(_NameTxt_fld *pFld);
  char *GetNoneString();
  int GetNationCode();
  bool IsApplyPcbangPrimium(const CPlayer *pUser);
  bool IsCashDBUseExtRef();
  bool IsCashDBInit();
  bool IsCashDBDSNSetted();
  void SetUnitPassiveValue(float *fUnitPv_DefFc);
  unsigned __int16 GetBillingForceCloseDelay();
  char *GetCashDBName();
  char *GetCashDBDBIP();
  char *GetCashDBID();
  char *GetCashDBPW();
  unsigned __int16 GetCashDBPort();
  char *GetWorldDBID();
  char *GetWorldDBPW();
  char *GetNationCodeStr();
  CHEAT_COMMAND *GetCheatTable();
  void SetCashDBDSN(char *szIP, char *szDBName, char *szAccount, char *szPassword, unsigned __int16 dwPort);
  void SetCashDBInitState();
  void OnConnectSession(unsigned int n);
  void OnDisConnectSession(unsigned int n);
  bool CheckEnterWorldRequest(unsigned int n, char *pBuf);
  bool IsPersonalFreeFixedAmountBillingType(__int16 *pDest1, __int16 *pDest2);
  void CreateComplete(CPlayer *pOne);
  bool IsNormalString(const char *szString);
  bool RecvGameGuardData(unsigned int n, _MSG_HEADER *pHeader, char *pBuff);
  void SendCashDBDSNRequest();
  void Loop();
  void NetClose(CPlayer *pOne);

  CNationSettingData *m_pData;
};
