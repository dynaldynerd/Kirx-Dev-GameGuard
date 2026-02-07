#include "pch.h"

#include "CNationSettingDataPH.h"

#include "CBillingPH.h"
#include "CashShop_str_fld.h"

CBilling *CNationSettingDataPH::CreateBilling()
{
  return new CBillingPH();
}

int CNationSettingDataPH::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_nphi_Price;
}

