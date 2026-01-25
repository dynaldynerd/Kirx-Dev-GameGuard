#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class __cppobj CNationSettingDataCN : public CNationSettingData
{
public:
  CBilling *CreateBilling() override;
};
