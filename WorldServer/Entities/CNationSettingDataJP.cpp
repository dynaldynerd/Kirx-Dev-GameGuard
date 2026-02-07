#include "pch.h"

#include "CNationSettingDataJP.h"

#include "CBillingJP.h"
#include "CashShop_str_fld.h"

CBilling *CNationSettingDataJP::CreateBilling()
{
  return new CBillingJP();
}

int CNationSettingDataJP::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_njpn_Price;
}

