#pragma once

#include "IdaCompat.h"

#include "CNationSettingFactory.h"

class __cppobj CNationSettingFactoryGB : public CNationSettingFactory
{
public:
  CNationSettingFactoryGB();
  CNationSettingData *Create(int iNationCode, const char *szNationCodeStr, bool bServiceMode) override;
};
