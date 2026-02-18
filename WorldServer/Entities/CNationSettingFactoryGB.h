#pragma once

#include "IdaCompat.h"

#include "CNationSettingFactory.h"

class  CNationSettingFactoryGB : public CNationSettingFactory
{
public:
  CNationSettingFactoryGB();
  CNationSettingData *Create(int iNationCode, const char *szNationCodeStr, bool bServiceMode) override;
};
