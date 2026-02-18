#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class  CNationSettingDataES : public CNationSettingData
{
public:
  int GetCashItemPrice(_CashShop_str_fld *pFld) override;
};
