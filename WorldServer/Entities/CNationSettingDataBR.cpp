#include "pch.h"

#include "CNationSettingDataBR.h"

#include "CBillingBR.h"
#include "CashShop_str_fld.h"

CBilling *CNationSettingDataBR::CreateBilling()
{
  return new CBillingBR();
}

int CNationSettingDataBR::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_nbra_Price;
}

