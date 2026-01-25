#include "pch.h"

#include "CConnNumPHMgr.h"

void CConnNumPHMgr::Init()
{
  m_nLastHour = 0;
  m_dwCheckLastTime = 0;
  m_cph.nUserCumPerMin = 0;
  m_cph.dwCounting = 0;
  m_cph.nMaxUserNum = 0;
  m_LastResult.nAveragePerHour = 0;
  m_LastResult.nMaxPerHour = 0;
  m_LastResult.nCount = 0;
}
