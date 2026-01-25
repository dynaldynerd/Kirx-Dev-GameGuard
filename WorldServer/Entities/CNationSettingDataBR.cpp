#include "pch.h"

#include "CNationSettingDataBR.h"

#include "CBillingBR.h"

CBilling *CNationSettingDataBR::CreateBilling()
{
  return new CBillingBR();
}

