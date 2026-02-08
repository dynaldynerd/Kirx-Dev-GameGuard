#pragma once

#include "IdaCompat.h"

#include "CNationSettingFactory.h"

class __cppobj CNationSettingFactoryRU : public CNationSettingFactory
{
public:
  CNationSettingFactoryRU();
  CNationSettingData *Create(int iNationCode, const char *szNationCodeStr, bool bServiceMode) override;
};
