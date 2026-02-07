#include "pch.h"

#include "CNationSettingDataES.h"

#include "CashShop_str_fld.h"

int CNationSettingDataES::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_nspa_Price;
}
