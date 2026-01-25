#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class __cppobj CNationSettingDataNULL : public CNationSettingData
{
public:
  CBilling *CreateBilling() override;
};
