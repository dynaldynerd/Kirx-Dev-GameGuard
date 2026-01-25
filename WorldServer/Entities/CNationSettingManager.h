#pragma once

#include "IdaCompat.h"

#include "CTSingleton.h"

class CNationSettingData;

class __cppobj CNationSettingManager : public CTSingleton<CNationSettingManager>
{
public:
  int Init(int nationCode, const char *nationCodeStr, bool serviceMode);

  CNationSettingData *m_pData;
};
