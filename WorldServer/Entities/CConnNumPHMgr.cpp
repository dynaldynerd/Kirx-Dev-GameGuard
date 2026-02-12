#include "pch.h"

#include "CConnNumPHMgr.h"

#include <ctime>
#include <mmsystem.h>

void CConnNumPHMgr::__cnt_per_h::Init()
{
  memset_0(this, 0, sizeof(*this));
}

int CConnNumPHMgr::__cnt_per_h::GetAverageUserNumPerHour()
{
  if (dwCounting)
  {
    return nUserCumPerMin / static_cast<int>(dwCounting);
  }
  return 0;
}

void CConnNumPHMgr::Init()
{
  m_nLastHour = 0;
  m_dwCheckLastTime = 0;
  m_cph.Init();
  m_LastResult.nAveragePerHour = 0;
  m_LastResult.nMaxPerHour = 0;
  m_LastResult.nCount = 0;
}

_USER_NUM_SHEET *CConnNumPHMgr::Check(int nLoginNum)
{
  bool hasResult = false;
  const DWORD currentTime = timeGetTime();
  if (currentTime - m_dwCheckLastTime > 0xEA60)
  {
    m_dwCheckLastTime = currentTime;
    m_cph.nUserCumPerMin += nLoginNum;
    ++m_cph.dwCounting;
    if (m_cph.nMaxUserNum < nLoginNum)
    {
      m_cph.nMaxUserNum = nLoginNum;
    }

    const int currentHour = GetCurHour();
    if (currentHour != m_nLastHour)
    {
      if (m_cph.nUserCumPerMin > 0)
      {
        m_LastResult.nAveragePerHour = m_cph.GetAverageUserNumPerHour();
        m_LastResult.nMaxPerHour = m_cph.nMaxUserNum;
        m_LastResult.nCount = static_cast<int>(m_cph.dwCounting);
        hasResult = true;
      }

      m_cph.Init();
      m_nLastHour = currentHour;
    }
  }

  if (hasResult)
  {
    return &m_LastResult;
  }

  return nullptr;
}

int CConnNumPHMgr::GetCurHour()
{
  __time64_t currentTime{};
  _time64(&currentTime);
  tm *localTimeInfo = _localtime64(&currentTime);
  if (!localTimeInfo)
  {
    return 0;
  }
  return localTimeInfo->tm_hour;
}
