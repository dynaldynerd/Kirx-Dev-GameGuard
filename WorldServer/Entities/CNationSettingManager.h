#pragma once

#include "IdaCompat.h"

#include "CTSingleton.h"

class CNationSettingData;

class __cppobj CNationSettingManager : public CTSingleton<CNationSettingManager>
{
public:
  CNationSettingData *m_pData;
};
