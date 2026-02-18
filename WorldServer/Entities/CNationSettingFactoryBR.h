#pragma once

#include "IdaCompat.h"

#include "CNationSettingFactory.h"

class  CNationSettingFactoryBR : public CNationSettingFactory
{
public:
  CNationSettingFactoryBR();
  CNationSettingData *Create(int iNationCode, const char *szNationCodeStr, bool bServiceMode) override;
};
