#include "pch.h"

#include "CNationSettingDataRU.h"

#include "CBillingRU.h"
#include "CashShop_str_fld.h"

CBilling *CNationSettingDataRU::CreateBilling()
{
  return new CBillingRU();
}

int CNationSettingDataRU::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_nrus_Price;
}

