#include "pch.h"

#include "C24Timer.h"
#include "WorldServerUtil.h"

C24Timer::C24Timer()
{
  Init();
}

C24Timer::~C24Timer()
{
}

void C24Timer::Init()
{
  m_dwBaseTickTime = GetLoopTime();
  m_dwBase24Time = static_cast<unsigned int>(GetCurrentMin() + 60 * GetCurrentHour());
}

__int64 C24Timer::Get24TimeFromTickTime(unsigned int dwTickTime)
{
  const unsigned int passMinutes = (dwTickTime - m_dwBaseTickTime) / 60000;
  return static_cast<__int64>(passMinutes + m_dwBase24Time);
}

