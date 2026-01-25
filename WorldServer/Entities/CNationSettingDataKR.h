#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class __cppobj CNationSettingDataKR : public CNationSettingData
{
public:
  CBilling *CreateBilling() override;
};
