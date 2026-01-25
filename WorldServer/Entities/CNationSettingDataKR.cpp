#include "pch.h"

#include "CNationSettingDataKR.h"

#include "CBillingKR.h"

CBilling *CNationSettingDataKR::CreateBilling()
{
  return new CBillingKR();
}

