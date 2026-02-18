#pragma once

#include "IdaCompat.h"

#include "CNationSettingFactory.h"

class  CNationSettingFactoryPH : public CNationSettingFactory
{
public:
  CNationSettingFactoryPH();
  CNationSettingData *Create(int iNationCode, const char *szNationCodeStr, bool bServiceMode) override;
};
