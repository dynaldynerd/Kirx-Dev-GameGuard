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

void CMyTimer::BeginTimerAddLapse(int dwTerm, unsigned int dwAddLapse)
{
  m_bOper = true;
  m_nTickTerm = dwTerm;
  m_dwTickOld = dwAddLapse + timeGetTime();
}

void CMyTimer::CountingAddTickOld(unsigned int dwAddGap)
{
  m_dwTickOld -= dwAddGap;
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

unsigned int CMyTimer::GetTerm()
{
  return static_cast<unsigned int>(m_nTickTerm);
}

void CMyTimer::TermTimeRun()
{
  m_dwTickOld = timeGetTime();
}

void CMyTimer::StopTimer()
{
  m_bOper = false;
}
