#include "pch.h"

#include "CActionPointSystemMgr.h"

#include <cstdio>
#include <cstring>
#include <ctime>

_action_point_system_ini::_action_point_system_ini()
{
  std::memset(this, 0, sizeof(*this));
}

CActionPointSystemMgr::CActionPointSystemMgr()
{
}

CActionPointSystemMgr::~CActionPointSystemMgr()
{
  // this is not a stub
}

CActionPointSystemMgr *CActionPointSystemMgr::Instance()
{
  static CActionPointSystemMgr s_instance;
  return &s_instance;
}

bool CActionPointSystemMgr::Initialize()
{
  for (int j = 0; j < 3; ++j)
  {
    m_bActive[j] = 0;
  }

  for (int k = 0; k < 3; ++k)
  {
    Load_Event_INI(&m_st_ini_list[k], static_cast<unsigned __int16>(k));
  }

  for (int m = 0; m < 3; ++m)
  {
    if (m_st_ini_list[m].m_bUse_event)
    {
      Check_Load_Event_Status(static_cast<unsigned __int8>(m), &m_st_ini_list[m]);
    }
  }
  return true;
}

void CActionPointSystemMgr::Load_Event_INI(_action_point_system_ini *pIni, unsigned __int16 wIndex)
{
  if (pIni == nullptr)
  {
    return;
  }

  const int index = wIndex;
  char sectionName[32]{};
  std::memset(sectionName, 0, sizeof(sectionName));
  sprintf_s(sectionName, sizeof(sectionName), "Action_Select_%d", index);

  pIni->m_bUse_event = false;
  pIni->m_EventTime[0] = 0;
  pIni->m_EventTime[1] = 0;

  const UINT useVal = GetPrivateProfileIntA(sectionName, "Use", 1, "./initialize/ActionPointSystem.ini");
  pIni->m_bUse_event = (useVal != 0);

  const UINT resetVal = GetPrivateProfileIntA(sectionName, "Reset", 1, "./initialize/ActionPointSystem.ini");
  pIni->m_bReset = (resetVal != 0);

  pIni->m_byActionCode =
    static_cast<unsigned __int8>(GetPrivateProfileIntA(sectionName, "Action_Code", 1, "./initialize/ActionPointSystem.ini"));

  UINT val = GetPrivateProfileIntA(sectionName, "BEGIN_YEAR", 0, "./initialize/ActionPointSystem.ini");
  if (!val)
  {
    pIni->m_bUse_event = false;
    return;
  }
  pIni->m_wYear[0] = static_cast<int>(val);

  val = GetPrivateProfileIntA(sectionName, "BEGIN_MONTH", 0, "./initialize/ActionPointSystem.ini");
  if (!val)
  {
    pIni->m_bUse_event = false;
    return;
  }
  pIni->m_byMonth[0] = static_cast<int>(val);

  val = GetPrivateProfileIntA(sectionName, "BEGIN_DAY", 0, "./initialize/ActionPointSystem.ini");
  if (!val)
  {
    pIni->m_bUse_event = false;
    return;
  }
  pIni->m_byDay[0] = static_cast<int>(val);

  val = GetPrivateProfileIntA(sectionName, "BEGIN_HOUR", 0, "./initialize/ActionPointSystem.ini");
  if (val >= 24)
  {
    pIni->m_bUse_event = false;
    return;
  }
  pIni->m_byHour[0] = static_cast<int>(val);

  val = GetPrivateProfileIntA(sectionName, "BEGIN_MINUTE", 0, "./initialize/ActionPointSystem.ini");
  if (val >= 60)
  {
    pIni->m_bUse_event = false;
    return;
  }
  pIni->m_byMinute[0] = static_cast<int>(val);

  std::tm tmStart{};
  tmStart.tm_year = pIni->m_wYear[0] - 1900;
  tmStart.tm_mon = pIni->m_byMonth[0] - 1;
  tmStart.tm_mday = pIni->m_byDay[0];
  tmStart.tm_hour = pIni->m_byHour[0];
  tmStart.tm_min = pIni->m_byMinute[0];
  tmStart.tm_sec = 0;
  tmStart.tm_isdst = -1;
  pIni->m_EventTime[0] = _mktime32(&tmStart);
  if (pIni->m_EventTime[0] == -1)
  {
    pIni->m_bUse_event = false;
    return;
  }

  val = GetPrivateProfileIntA(sectionName, "END_YEAR", 0, "./initialize/ActionPointSystem.ini");
  if (!val)
  {
    pIni->m_bUse_event = false;
    return;
  }
  pIni->m_wYear[1] = static_cast<int>(val);

  val = GetPrivateProfileIntA(sectionName, "END_MONTH", 0, "./initialize/ActionPointSystem.ini");
  if (!val)
  {
    pIni->m_bUse_event = false;
    return;
  }
  pIni->m_byMonth[1] = static_cast<int>(val);

  val = GetPrivateProfileIntA(sectionName, "END_DAY", 0, "./initialize/ActionPointSystem.ini");
  if (!val)
  {
    pIni->m_bUse_event = false;
    return;
  }
  pIni->m_byDay[1] = static_cast<int>(val);

  val = GetPrivateProfileIntA(sectionName, "END_HOUR", 0, "./initialize/ActionPointSystem.ini");
  if (val >= 24)
  {
    pIni->m_bUse_event = false;
    return;
  }
  pIni->m_byHour[1] = static_cast<int>(val);

  val = GetPrivateProfileIntA(sectionName, "END_MINUTE", 0, "./initialize/ActionPointSystem.ini");
  if (val >= 60)
  {
    pIni->m_bUse_event = false;
    return;
  }
  pIni->m_byMinute[1] = static_cast<int>(val);

  std::tm tmEnd{};
  tmEnd.tm_year = pIni->m_wYear[1] - 1900;
  tmEnd.tm_mon = pIni->m_byMonth[1] - 1;
  tmEnd.tm_mday = pIni->m_byDay[1];
  tmEnd.tm_hour = pIni->m_byHour[1];
  tmEnd.tm_min = pIni->m_byMinute[1];
  tmEnd.tm_sec = 0;
  tmEnd.tm_isdst = -1;
  pIni->m_EventTime[1] = _mktime32(&tmEnd);
  if (pIni->m_EventTime[1] == -1)
  {
    pIni->m_bUse_event = false;
  }
}

void CActionPointSystemMgr::Check_Load_Event_Status(unsigned __int8 byActionCode, _action_point_system_ini *pIni)
{
  if (!pIni->m_bUse_event)
  {
    return;
  }

  __time32_t now{};
  _time32(&now);
  if (now > pIni->m_EventTime[1])
  {
    SetEventStatus(byActionCode, 0);
    return;
  }

  if (now < pIni->m_EventTime[0] || now > pIni->m_EventTime[1])
  {
    if (now < pIni->m_EventTime[0])
    {
      SetEventStatus(byActionCode, 1);
    }
    return;
  }

  const int remaining = pIni->m_EventTime[1] - now;
  if (remaining <= 0)
  {
    SetEventStatus(byActionCode, 0);
    return;
  }

  SetEventStatus(byActionCode, 2);
}

void CActionPointSystemMgr::Check_Event_Status()
{
  for (int j = 0; j < 3; ++j)
  {
    const unsigned __int8 status = GetEventStatus(static_cast<unsigned __int8>(j));
    __time32_t now{};
    _time32(&now);
    if (status)
    {
      if (status == 1)
      {
        if (now >= m_st_ini_list[j].m_EventTime[0])
        {
          SetEventStatus(static_cast<unsigned __int8>(j), 2);
        }
      }
      else if (status == 2)
      {
        if (m_st_ini_list[j].m_EventTime[1] <= now)
        {
          SetEventStatus(static_cast<unsigned __int8>(j), 3);
        }
      }
      else if (status == 3 && m_st_ini_list[j].m_EventTime[1] <= now)
      {
        SetEventStatus(static_cast<unsigned __int8>(j), 3);
      }
    }
    else if (!m_st_ini_list[j].m_bUse_event)
    {
      SetEventStatus(static_cast<unsigned __int8>(j), 0);
    }
  }
}

void CActionPointSystemMgr::Check_Loop()
{
  Check_Event_Status();
}

void CActionPointSystemMgr::SetEventStatus(unsigned __int8 byActionCode, unsigned __int8 byStatus)
{
  m_bActive[byActionCode] = byStatus;
}

unsigned __int8 CActionPointSystemMgr::GetEventStatus(unsigned __int8 byActionCode)
{
  return m_bActive[byActionCode];
}

bool CActionPointSystemMgr::IsSystemEnable(unsigned __int8 byActionCode)
{
  return m_st_ini_list[byActionCode].m_bUse_event;
}

bool CActionPointSystemMgr::IsPointReset(unsigned __int8 byActionCode)
{
  return m_st_ini_list[byActionCode].m_bReset;
}
