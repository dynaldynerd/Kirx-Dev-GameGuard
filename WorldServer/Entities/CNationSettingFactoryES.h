#pragma once

#include "IdaCompat.h"

#include "CNationSettingFactory.h"

class __cppobj CNationSettingFactoryES : public CNationSettingFactory
{
public:
  CNationSettingFactoryES();
  CNationSettingData *Create(int iNationCode, const char *szNationCodeStr, bool bServiceMode) override;
};
