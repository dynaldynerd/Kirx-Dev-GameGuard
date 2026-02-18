#pragma once

#include "IdaCompat.h"

#include "CNationSettingFactory.h"

class  CNationSettingFactoryJP : public CNationSettingFactory
{
public:
  CNationSettingFactoryJP();
  CNationSettingData *Create(int iNationCode, const char *szNationCodeStr, bool bServiceMode) override;
};
