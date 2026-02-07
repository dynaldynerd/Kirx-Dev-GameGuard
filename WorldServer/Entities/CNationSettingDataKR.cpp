#include "pch.h"

#include "CNationSettingDataKR.h"

#include "CBillingKR.h"
#include "CashShop_str_fld.h"

CBilling *CNationSettingDataKR::CreateBilling()
{
  return new CBillingKR();
}

int CNationSettingDataKR::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_nkor_Price;
}

