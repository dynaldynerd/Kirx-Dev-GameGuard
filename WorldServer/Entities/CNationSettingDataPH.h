#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class __cppobj CNationSettingDataPH : public CNationSettingData
{
public:
  CBilling *CreateBilling() override;
  int GetCashItemPrice(_CashShop_str_fld *pFld) override;
};
