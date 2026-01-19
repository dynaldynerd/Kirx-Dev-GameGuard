#pragma once

#include "CBilling.h"
#include "CTSingleton.h"

class __cppobj CBillingManager : public CTSingleton<CBillingManager>
{
public:
  CBilling *m_pBill;
};

