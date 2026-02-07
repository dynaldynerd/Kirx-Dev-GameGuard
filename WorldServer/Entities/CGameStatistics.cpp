#include "pch.h"

#include "CGameStatistics.h"

#include <cstdio>
#include <windows.h>

#include "WorldServerUtil.h"

CGameStatistics::_map::_map()
  : pMapName(nullptr),
    dwMaxHourPerMap_Hour(0)
{
}

CGameStatistics::_DAY::_DAY()
{
  for (int i = 0; i < 100; ++i)
  {
    new (&MaxHourPerMap_Hour[i]) _map();
  }
  init();
}

void CGameStatistics::_DAY::init()
{
  memset_0(this, 0, sizeof(CGameStatistics::_DAY));
}

CGameStatistics::CGameStatistics()
{
  CreateDirectoryA("..\\ZoneServerLog\\Statistics", nullptr);
  CreateDirectoryA("..\\ZoneServerLog\\Statistics\\Day", nullptr);
  m_day.init();
}

void CGameStatistics::Init()
{
  m_day.init();
}

void CGameStatistics::ConvertDay(char *pszWorldName)
{
  WriteDayData(pszWorldName);
  m_day.init();
}

CGameStatistics::_DAY *CGameStatistics::CurWriteData()
{
  return &this->m_day;
}

void CGameStatistics::WriteDayData(char *pszWorldName)
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  char filePath[128]{};
  sprintf_s(
    filePath,
    "..\\ZoneServerLog\\Statistics\\Day\\%s_StatisticsDay%d_%d_Time%d_%d_%d.txt",
    pszWorldName,
    systemTime.wMonth,
    systemTime.wDay,
    systemTime.wHour,
    systemTime.wMinute,
    systemTime.wSecond);

  FILE *stream = fopen(filePath, "at");
  if (!stream)
  {
    return;
  }

  fprintf(stream, "eder map >> %d \n", m_day.dwEderEnter_Evt);
  fprintf(stream, "peak time >> hour: %d user(avg): %d \n", m_day.dwMaxUserHour_Hour, m_day.dwMaxUser_Hour);

  for (int j = 0; j < 100; ++j)
  {
    _map *entry = &m_day.MaxHourPerMap_Hour[j];
    if (!entry->pMapName)
    {
      break;
    }
    fprintf(stream, "\t%s map >> user: %d \n", entry->pMapName, entry->dwMaxHourPerMap_Hour);
  }

  fprintf(stream, "drop std item >> %d \n", m_day.dwDropStdItem_Evt);
  fprintf(stream, "drop rare item >> %d \n", m_day.dwDropRareItem_Evt);
  fprintf(stream, "upgrade 4Mu success >> %d \n", m_day.dw4MuUpgradeSucc_Evt);
  fprintf(stream, "upgrade 5Mu success >> %d \n", m_day.dw5MuUpgradeSucc_Evt);
  fprintf(stream, "upgrade 4Eun success >> %d \n", m_day.dw4EunUpgradeSucc_Evt);
  fprintf(stream, "upgrade 5Eun success >> %d \n", m_day.dw5EunUpgradeSucc_Evt);
  fprintf(stream, "upgrade 4Ja success >> %d \n", m_day.dw4JaUpgradeSucc_Evt);
  fprintf(stream, "upgrade 5Ja success >> %d \n", m_day.dw5JaUpgradeSucc_Evt);
  fprintf(stream, "use daepok >> %d \n", m_day.dwDaePokUse_Evt);

  fclose(stream);
}
