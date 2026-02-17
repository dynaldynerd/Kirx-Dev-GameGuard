#include "pch.h"

#include "CAITimer.h"

#include <mmsystem.h>

CAITimer::CAITimer()
{
  m_BefTime = 0;
  m_Delay = 0;
  m_DDelay = 0;
}

void CAITimer::Init(unsigned int delay)
{
  m_Delay = delay;
  m_DDelay = delay;
}

void CAITimer::Set(unsigned int delay)
{
  if (delay)
  {
    m_Delay = delay;
  }
  else
  {
    m_Delay = m_DDelay;
  }
  m_BefTime = timeGetTime();
}

__int64 CAITimer::Check()
{
  if (m_Delay)
  {
    const unsigned int now = timeGetTime();
    const int elapsed = static_cast<int>(now - m_BefTime);
    if (m_Delay < static_cast<unsigned int>(elapsed))
    {
      m_Delay = m_DDelay;
      m_BefTime = now;
      return 1;
    }
    if (elapsed < 0)
    {
      m_BefTime = 0;
    }
  }
  return 0;
}

