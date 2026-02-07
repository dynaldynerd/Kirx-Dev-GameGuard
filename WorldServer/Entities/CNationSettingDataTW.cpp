#include "pch.h"

#include "CNationSettingDataTW.h"

#include "CashShop_str_fld.h"

int CNationSettingDataTW::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_ntpe_Price;
}
