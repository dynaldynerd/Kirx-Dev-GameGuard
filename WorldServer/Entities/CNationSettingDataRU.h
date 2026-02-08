#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class __cppobj CNationSettingDataRU : public CNationSettingData
{
public:
  CashDbWorker *CreateWorker() override;
  CBilling *CreateBilling() override;
  int GetCashItemPrice(_CashShop_str_fld *pFld) override;
};
