#include "pch.h"

#include "CMyTimer.h"

#include <mmsystem.h>

CMyTimer::CMyTimer()
  : m_nTickTerm(0),
    m_dwTickOld(0),
    m_bOper(false)
{
}

void CMyTimer::BeginTimer(unsigned int tickTerm)
{
  m_nTickTerm = static_cast<int>(tickTerm);
  m_dwTickOld = GetTickCount();
  m_bOper = true;
}

bool CMyTimer::CountingTimer()
{
  if (!m_bOper)
  {
    return false;
  }

  const DWORD now = timeGetTime();
  const int delta = static_cast<int>(now - m_dwTickOld);
  if (delta <= m_nTickTerm)
  {
    if (delta < 0)
    {
      m_dwTickOld = 0;
    }
    return false;
  }

  m_dwTickOld = now;
  return true;
}
