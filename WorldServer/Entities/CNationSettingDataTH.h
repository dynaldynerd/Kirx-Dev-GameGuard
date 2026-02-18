#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class  CNationSettingDataTH : public CNationSettingData
{
public:
  int GetCashItemPrice(_CashShop_str_fld *pFld) override;
};
