#include "pch.h"

#include "CNationSettingDataGB.h"

#include "CashShop_str_fld.h"

int CNationSettingDataGB::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_ngbn_Price;
}
