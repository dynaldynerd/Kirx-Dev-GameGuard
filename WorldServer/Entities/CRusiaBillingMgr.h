#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "param_cash.h"

#include <windows.h>

class  CRusiaBillingMgr
{
public:
  CRusiaBillingMgr();
  ~CRusiaBillingMgr();

  CRusiaBillingMgr *Instance();
  void Release();
  __int64 Init();
  __int64 Free();
  bool ConfigUserODBC(char *szDSN, char *szServer, char *szDatabase, unsigned __int16 wPort, bool bTrustedConnection);
  bool LoadINIFile();
  void ArrangeString(char *szDest, char *szSorc, char cToken);
  char *dhExtractSubString(char *szSub, char *szFull, char cToken);
  void dhRExtractSubString(char *szSub, char *szFull, char cToken);
  void DeleteMem();
  __int64 CallFunc_RFOnline_Auth(_param_cash_select *rParam);
  __int64 CallFunc_Item_Buy(_param_cash_update *rParam, int nIdx);
  __int64 CallFunc_Item_Cancel(_param_cash_rollback::__list *list, char *szUserID);

  static CRusiaBillingMgr *m_pRusiaBill;

  CLogFile m_logBill;
  HINSTANCE m_hDll;
  char m_lpszDSN[50];
  char m_lpszServer[50];
  char m_lpszDataBase[50];
  char m_lpszPort[50];
  char m_lpszAccount[50];
  char m_lpszPassword[50];
};

static_assert(sizeof(CRusiaBillingMgr) == 0x1F0, "CRusiaBillingMgr size mismatch");

