#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class  CNationSettingDataUS : public CNationSettingData
{
public:
  int GetCashItemPrice(_CashShop_str_fld *pFld) override;
};
