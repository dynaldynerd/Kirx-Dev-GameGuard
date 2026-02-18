#pragma once

#include "IdaCompat.h"

struct _SYSTEMTIME;
class CUserDB;

class  CBilling
{
public:
  CBilling();
  virtual ~CBilling();

  virtual void Login(CUserDB *pUserDB);
  virtual void Alive(CUserDB *pUserDB);
  virtual void Logout(CUserDB *pUserDB);
  virtual void BillingClose(char *szID);
  virtual bool SendMsg_Login(char *szID, char *szIP, char *szCMS, short iType, _SYSTEMTIME *pstEndDate, int lRemainTime);
  void Start();
  void SendMsg_StartBilling();
  void SendMsg_CurAllUserLogin();
  void SetOper(bool bOper);
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

  bool m_bOper;
};

