#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class __cppobj CNationSettingDataBR : public CNationSettingData
{
public:
  CBilling *CreateBilling() override;
};
