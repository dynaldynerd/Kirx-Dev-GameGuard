#include "pch.h"

#include "CNationSettingDataCN.h"

#include "CBillingCN.h"

CBilling *CNationSettingDataCN::CreateBilling()
{
  return new CBillingCN();
}

