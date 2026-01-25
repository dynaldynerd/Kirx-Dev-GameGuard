#pragma once

#include "IdaCompat.h"

#include "CNationSettingData.h"

class __cppobj CNationSettingDataJP : public CNationSettingData
{
public:
  CBilling *CreateBilling() override;
};
