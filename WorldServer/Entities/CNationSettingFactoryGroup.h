#pragma once

#include "IdaCompat.h"

#include "CHashMapPtrPool.h"
#include "CNationSettingFactory.h"

class CNationSettingData;

class  CNationSettingFactoryGroup
{
public:
  CNationSettingFactoryGroup();
  ~CNationSettingFactoryGroup();

  int Init();
  CNationSettingData *Create(int iNationCode, const char *szNationCodeStr, bool bServiceMode);

  CHashMapPtrPool<int, CNationSettingFactory> m_kPool;
};
