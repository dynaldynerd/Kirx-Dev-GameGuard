#include "pch.h"

#include "CMyTimer.h"

void CMyTimer::BeginTimer(unsigned int tickTerm)
{
  m_nTickTerm = static_cast<int>(tickTerm);
  m_dwTickOld = GetTickCount();
  m_bOper = true;
}
