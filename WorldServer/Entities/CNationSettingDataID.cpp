#include "pch.h"

#include "CNationSettingDataID.h"

#include "CBillingID.h"
#include "CashShop_str_fld.h"

CBilling *CNationSettingDataID::CreateBilling()
{
  return new CBillingID();
}

int CNationSettingDataID::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_nina_Price;
}

