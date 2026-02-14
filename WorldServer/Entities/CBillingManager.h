#pragma once

#include "CBilling.h"
#include "CTSingleton.h"

class CUserDB;

class __cppobj CBillingManager : public CTSingleton<CBillingManager>
{
public:
  CBillingManager();
  ~CBillingManager();

  bool Init();
  bool LoadINI();
  void Start();
  void Login(CUserDB *pUserDB);
  void Alive(CUserDB *pUserDB);
  void Logout(CUserDB *pUserDB);
  bool IsOperate();
  void BillingClose(char *szID);
  void Remaintime_Personal(char *szID, __int16 iType, int lRemaintime, _SYSTEMTIME *pstEndDate);
  void Remaintime_PCBang(char *szCMSCode, __int16 iType, int lRemaintime, _SYSTEMTIME *pstEndDate);
  void Change_BillingType(
    char *szID,
    char *szCMSCode,
    __int16 iType,
    int lRemainTime,
    _SYSTEMTIME *pstEndDate,
    unsigned __int8 byReason);
  void Expire_Personal(char *szID);
  void Expire_PCBang(char *szCMS);
  void Expire_IPOverflow(char *szID);
  void Change_Primium(char *szID, bool bResult);
  void SendMsg_ZoneAliveCheck(unsigned int dwData);

  CBilling *m_pBill;
};

