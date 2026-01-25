#include "pch.h"

#include "CNationSettingDataRU.h"

#include "CBillingRU.h"

CBilling *CNationSettingDataRU::CreateBilling()
{
  return new CBillingRU();
}

