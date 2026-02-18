#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class  CNationSettingDataBR : public CNationSettingData
{
public:
  CBilling *CreateBilling() override;
  int GetCashItemPrice(_CashShop_str_fld *pFld) override;
};
