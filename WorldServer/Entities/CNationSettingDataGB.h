#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class __cppobj CNationSettingDataGB : public CNationSettingData
{
public:
  int GetCashItemPrice(_CashShop_str_fld *pFld) override;
};
