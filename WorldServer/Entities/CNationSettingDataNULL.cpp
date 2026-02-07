#include "pch.h"

#include "CNationSettingDataNULL.h"

#include "CashShop_str_fld.h"

CBilling *CNationSettingDataNULL::CreateBilling()
{
  return nullptr;
}

int CNationSettingDataNULL::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  (void)pFld;
  // this is not a stub
  return 0;
}

