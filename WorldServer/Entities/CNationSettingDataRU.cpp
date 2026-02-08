#include "pch.h"

#include "CNationSettingDataRU.h"

#include "CBillingRU.h"
#include "CCashDbWorkerRU.h"
#include "CashShop_str_fld.h"

#include <new>

CashDbWorker *CNationSettingDataRU::CreateWorker()
{
  auto *worker = static_cast<CCashDbWorkerRU *>(operator new(0x230uLL));
  if (!worker)
  {
    return nullptr;
  }
  new (worker) CCashDbWorkerRU();
  return worker;
}

CBilling *CNationSettingDataRU::CreateBilling()
{
  return new CBillingRU();
}

int CNationSettingDataRU::GetCashItemPrice(_CashShop_str_fld *pFld)
{
  return pFld->m_nrus_Price;
}

