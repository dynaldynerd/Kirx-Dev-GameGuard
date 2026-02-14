#pragma once

#include "IdaCompat.h"

class __cppobj CGameStatistics
{
public:
  struct __cppobj _map
  {
    const char *pMapName;
    unsigned int dwMaxHourPerMap_Hour;

    _map();
  };

  struct __cppobj _DAY
  {
    _map MaxHourPerMap_Hour[100];
    unsigned int dwEderEnter_Evt;
    unsigned int dwMaxUserHour_Hour;
    unsigned int dwMaxUser_Hour;
    unsigned int dwDropStdItem_Evt;
    unsigned int dwDropRareItem_Evt;
    unsigned int dw4MuUpgradeSucc_Evt;
    unsigned int dw5MuUpgradeSucc_Evt;
    unsigned int dw4EunUpgradeSucc_Evt;
    unsigned int dw5EunUpgradeSucc_Evt;
    unsigned int dw4JaUpgradeSucc_Evt;
    unsigned int dw5JaUpgradeSucc_Evt;
    unsigned int dwDaePokUse_Evt;

    _DAY();
    void init();
  };

  CGameStatistics();
  ~CGameStatistics();
  void Init();
  void ConvertDay(char *pszWorldName);
  void WriteDayData(char *pszWorldName);
  _DAY *CurWriteData();

  _DAY m_day;
};

