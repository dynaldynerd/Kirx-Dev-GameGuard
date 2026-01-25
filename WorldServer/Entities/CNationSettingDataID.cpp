#include "pch.h"

#include "CNationSettingDataID.h"

#include "CBillingID.h"

CBilling *CNationSettingDataID::CreateBilling()
{
  return new CBillingID();
}

