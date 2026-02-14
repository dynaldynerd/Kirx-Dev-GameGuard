#include "pch.h"

#include "CNationSettingDataNULL.h"

#include "CashShop_str_fld.h"

CNationSettingDataNULL CNationSettingDataNULL::ms_NULL{};

CBilling *CNationSettingDataNULL::CreateBilling()
{
  return nullptr;
}

int CNationSettingDataNULL::GetCashItemPrice(_CashShop_str_fld *pFld)
{
// this is not a stub
  return 0;
}

