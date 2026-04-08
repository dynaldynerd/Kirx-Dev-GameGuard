#include "pch.h"

#include "CNationSettingDataTH.h"

#include "CashShop_str_fld.h"

int CNationSettingDataTH::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_ntpe_Price;
}
