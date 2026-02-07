#pragma once

#include "IdaCompat.h"

struct _SYSTEMTIME;
class CUserDB;

class __cppobj CBilling
{
public:
  virtual ~CBilling() = default;
  virtual void Login(CUserDB *) {}
  virtual void Alive(CUserDB *) {}
  virtual void Logout(CUserDB *) {}
  virtual void BillingClose(char *) {}
  virtual bool SendMsg_Login(char *, char *, char *, short, _SYSTEMTIME *, int) { return false; }
  void Start();
  void SendMsg_StartBilling();
  void SendMsg_CurAllUserLogin();

  bool m_bOper;
};

