#pragma once

#include "IdaCompat.h"

#include "CNationSettingFactory.h"

class __cppobj CNationSettingFactoryTH : public CNationSettingFactory
{
public:
  CNationSettingFactoryTH();
  CNationSettingData *Create(int iNationCode, const char *szNationCodeStr, bool bServiceMode) override;
};
