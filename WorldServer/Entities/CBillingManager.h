#pragma once

#include "CBilling.h"
#include "CTSingleton.h"

class __cppobj CBillingManager : public CTSingleton<CBillingManager>
{
public:
  CBillingManager();
  bool Init();
  bool LoadINI();

  CBilling *m_pBill;
};

