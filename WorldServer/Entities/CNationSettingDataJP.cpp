#include "pch.h"

#include "CNationSettingDataJP.h"

#include "CBillingJP.h"

CBilling *CNationSettingDataJP::CreateBilling()
{
  return new CBillingJP();
}

