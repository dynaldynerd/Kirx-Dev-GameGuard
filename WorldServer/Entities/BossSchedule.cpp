#include "pch.h"

#include "CMainThread.h"
#include "CBossMonsterScheduleSystem.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

const char *BossSchedule::ms_KeyPair[3] = {"MOBCODE", "LTTM", "LIVE"};

ScheduleMSG::ScheduleMSG()
{
  Init();
}

void ScheduleMSG::Init()
{
  memset_0(this, 0, sizeof(ScheduleMSG));
  m_byKey = 3;
}

BossSchedule::BossSchedule()
{
  m_LastRespawnSystemTime = ATL::CTime();
  memset_0(this, 0, sizeof(BossSchedule));
}

BossSchedule::~BossSchedule()
{
  if (m_pMonAct)
  {
    m_pMonAct->SetBossSchedule(nullptr);
  }
}

unsigned __int16 BossSchedule::Make_LiveCount(char *strTimeValue)
{
  if (strTimeValue)
  {
    return static_cast<unsigned __int16>(atoi(strTimeValue));
  }
  return 0;
}

ATL::CTime *BossSchedule::Make_LastTimeRespawnSystemTime(ATL::CTime *result, char *strTimeValue)
{
  if (strTimeValue)
  {
    std::vector<std::string> stringlist;
    if (SplitString(strTimeValue, "-", &stringlist) == 7)
    {
      const int nSec = atoi(stringlist[5].c_str());
      const int nMin = atoi(stringlist[4].c_str());
      const int nHour = atoi(stringlist[3].c_str());
      const int nDay = atoi(stringlist[2].c_str());
      const int nMonth = atoi(stringlist[1].c_str());
      const int nYear = atoi(stringlist[0].c_str());
      *result = ATL::CTime(nYear, nMonth, nDay, nHour, nMin, nSec, -1);
    }
    else
    {
      *result = ATL::CTime(0);
    }
    return result;
  }
  *result = ATL::CTime(0);
  return result;
}

bool BossSchedule::Make_LiveCountString(char *strBuff, int nBuffSize)
{
  if (!strBuff)
  {
    return false;
  }
  sprintf_s(strBuff, nBuffSize, "%d", m_LiveCount);
  return true;
}

bool BossSchedule::Make_LastTimeRespawnSystemTimeString(char *strBuff, int nBuffSize)
{
  if (!strBuff)
  {
    return false;
  }

  const int minute = m_LastRespawnSystemTime.GetMinute();
  const int second = m_LastRespawnSystemTime.GetSecond();
  const int minuteRepeat = m_LastRespawnSystemTime.GetMinute();
  const int hour = m_LastRespawnSystemTime.GetHour();
  const int day = m_LastRespawnSystemTime.GetDay();
  const int month = m_LastRespawnSystemTime.GetMonth();
  const int year = m_LastRespawnSystemTime.GetYear();

  sprintf_s(
    strBuff,
    nBuffSize,
    "%04d-%02d-%02d-%02d-%02d-%02d-%03d",
    year,
    month,
    day,
    hour,
    minuteRepeat,
    second,
    minute);
  return true;
}

void BossSchedule::Save_LiveCount(unsigned __int16 wCount)
{
  if (!m_pParent)
  {
    return;
  }
  if (!m_pParent->m_pSystem)
  {
    return;
  }

  ScheduleMSG *pTask = m_pParent->m_pSystem->m_MSG_POOL.PopEmpty();
  if (!pTask)
  {
    return;
  }

  pTask->Init();
  m_LiveCount = wCount;
  pTask->m_byKey = 2;
  strcpy_s(pTask->m_strSection, sizeof(pTask->m_strSection), m_strSection);
  pTask->m_wIniFileIndex = static_cast<unsigned __int16>(m_pParent->m_nIndex);
  if (Make_LiveCountString(pTask->m_strValue, 64))
  {
    m_pParent->m_pSystem->m_MSG_POOL.PushProc(pTask);
  }
}

void BossSchedule::Save_LastRespawnSystemTime(ATL::CTime *systime)
{
  if (!m_pParent)
  {
    return;
  }
  if (!m_pParent->m_pSystem)
  {
    return;
  }

  ScheduleMSG *pTask = m_pParent->m_pSystem->m_MSG_POOL.PopEmpty();
  if (!pTask)
  {
    return;
  }

  pTask->Init();
  m_LastRespawnSystemTime = *systime;
  pTask->m_byKey = 1;
  strcpy_s(pTask->m_strSection, sizeof(pTask->m_strSection), m_strSection);
  pTask->m_wIniFileIndex = static_cast<unsigned __int16>(m_pParent->m_nIndex);
  if (Make_LastTimeRespawnSystemTimeString(pTask->m_strValue, 64))
  {
    m_pParent->m_pSystem->m_MSG_POOL.PushProc(pTask);
  }
}

BossSchedule_Map::BossSchedule_Map()
{
  memset_0(this, 0, sizeof(BossSchedule_Map));
}

BossSchedule_Map::~BossSchedule_Map()
{
  Clear();
}

void BossSchedule_Map::Clear()
{
  if (m_ScheduleList)
  {
    for (int j = 0; j < m_nCount; ++j)
    {
      if (m_ScheduleList[j])
      {
        delete m_ScheduleList[j];
      }
    }
    operator delete[](m_ScheduleList);
    m_ScheduleList = nullptr;
  }
}

bool BossSchedule_Map::SaveAll()
{
  if (m_ScheduleList)
  {
    for (int j = 0; j < m_nCount; ++j)
    {
      BossSchedule *pSchedule = m_ScheduleList[j];
      if (!pSchedule)
      {
        return false;
      }
      if (!m_pSystem)
      {
        return false;
      }
      m_pSystem->Savechedule(this, pSchedule);
    }
  }
  return true;
}

bool BossSchedule_Map::LoadAll()
{
  Clear();
  m_nCount = static_cast<int>(m_INIFile.GetSectionSize());
  if (m_nCount <= 0)
  {
    return false;
  }
  if (!m_pSystem || !m_pSystem->m_pMapOper)
  {
    return false;
  }

  m_ScheduleList = static_cast<BossSchedule **>(operator new[](saturated_mul(static_cast<size_t>(m_nCount), sizeof(BossSchedule *))));
  memset_0(m_ScheduleList, 0, sizeof(BossSchedule *) * m_nCount);

  for (unsigned int dwIndex = 0; dwIndex < static_cast<unsigned int>(m_nCount); ++dwIndex)
  {
    INI_Section *pSection = m_INIFile.GetSection(dwIndex);
    if (!pSection)
    {
      return false;
    }
    BossSchedule *schedule = m_pSystem->LoadSchedule(this, pSection);
    m_ScheduleList[dwIndex] = schedule;
  }
  return true;
}

BossSchedule_TBL::BossSchedule_TBL()
{
  memset_0(this, 0, sizeof(BossSchedule_TBL));
}

BossSchedule_TBL::~BossSchedule_TBL()
{
  if (m_MapScheduleList)
  {
    for (int j = 0; j < m_nCount; ++j)
    {
      if (m_MapScheduleList[j])
      {
        delete m_MapScheduleList[j];
      }
    }
    operator delete[](m_MapScheduleList);
  }
}
