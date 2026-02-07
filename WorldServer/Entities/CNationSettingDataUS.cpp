#include "pch.h"

#include "CNationSettingDataUS.h"

#include "CashShop_str_fld.h"

int CNationSettingDataUS::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_ngbn_Price;
}
