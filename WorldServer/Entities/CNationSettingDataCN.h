#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class  CNationSettingDataCN : public CNationSettingData
{
public:
  CBilling *CreateBilling() override;
  int GetCashItemPrice(_CashShop_str_fld *pFld) override;
};
