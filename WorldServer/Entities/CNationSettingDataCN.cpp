#include "pch.h"

#include "CNationSettingDataCN.h"

#include "CBillingCN.h"
#include "CashShop_str_fld.h"

CBilling *CNationSettingDataCN::CreateBilling()
{
  return new CBillingCN();
}

int CNationSettingDataCN::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_nchn_Price;
}

