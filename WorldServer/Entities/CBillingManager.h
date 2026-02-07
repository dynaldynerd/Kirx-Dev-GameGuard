#pragma once

#include "CBilling.h"
#include "CTSingleton.h"

class CUserDB;

class __cppobj CBillingManager : public CTSingleton<CBillingManager>
{
public:
  CBillingManager();
  bool Init();
  bool LoadINI();
  void Start();
  void Logout(CUserDB *pUserDB);

  CBilling *m_pBill;
};

