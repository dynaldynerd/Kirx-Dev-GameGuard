#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class  CNationSettingDataNULL : public CNationSettingData
{
public:
  static CNationSettingDataNULL ms_NULL;

  CBilling *CreateBilling() override;
  int GetCashItemPrice(_CashShop_str_fld *pFld) override;
};
