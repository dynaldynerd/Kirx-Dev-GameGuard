#include "pch.h"

#include "CNationSettingDataPH.h"

#include "CBillingPH.h"

CBilling *CNationSettingDataPH::CreateBilling()
{
  return new CBillingPH();
}

