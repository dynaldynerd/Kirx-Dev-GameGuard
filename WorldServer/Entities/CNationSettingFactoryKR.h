#pragma once

#include "IdaCompat.h"

#include "CNationSettingFactory.h"

class __cppobj CNationSettingFactoryKR : public CNationSettingFactory
{
public:
  CNationSettingFactoryKR();
  CNationSettingData *Create(int iNationCode, const char *szNationCodeStr, bool bServiceMode) override;
};
