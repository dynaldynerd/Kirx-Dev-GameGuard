#include "pch.h"

#include "CTimer.h"

#include "WorldServerUtil.h"
#include <mmsystem.h>

CTimer::CTimer()
{
  std::memset(this, 0, sizeof(CTimer));
  mMinFPS = 1.0f;
}

void CTimer::CalculateTime()
{
  if (!m_bTimerInitialized)
  {
    m_bTimerInitialized = 1;
    LARGE_INTEGER frequency{};
    if (!QueryPerformanceFrequency(&frequency))
    {
      m_qwTicksPerSec = 1000;
    }
    else
    {
      m_qwTicksPerSec = static_cast<unsigned __int64>(frequency.QuadPart);
    }

    m_fSecsPerFrame = 0.001f;
    m_fTicksPerSec = static_cast<float>(static_cast<int>(m_qwTicksPerSec));
    m_qwTicksPerFrame = m_qwTicksPerSec / 1000ULL;
    LARGE_INTEGER ticks = GetTicks();
    m_qwTicks = ticks.QuadPart;
    m_qwStartTicks = ticks.QuadPart;
  }

  const unsigned int now = timeGetTime();
  ++mLoopCnt;
  mLoopHop += now - mOldTime;
  if (mLoopHop > 10)
  {
    const float hop = static_cast<float>(static_cast<int>(mLoopHop));
    const int loopCount = mLoopCnt;
    mLoopCnt = 0;
    mLoopHop = 0;
    mLoopTime = (hop / static_cast<float>(loopCount)) / 1000.0f;
  }

  mOldTime = now;
  if (mLoopTime > mMinFPS || mLoopTime < 0.0f)
  {
    mLoopTime = mMinFPS;
  }

  ++mLoopFPSCnt;
  const float fpsTime = mFPSTime + mLoopTime;
  mFPSTime = fpsTime;
  mRealTime += mLoopTime;
  if (fpsTime > 1.0f)
  {
    const int loopFpsCount = mLoopFPSCnt;
    mFPSTime = 0.0f;
    mLoopFPSCnt = 0;
    mFPS = static_cast<float>(loopFpsCount) / fpsTime;
  }
}

float CTimer::GetDuration()
{
  LARGE_INTEGER ticks = GetTicks();
  return static_cast<float>(static_cast<int>(ticks.LowPart - static_cast<unsigned int>(m_qwStartTicks)))
       / static_cast<float>(static_cast<int>(m_qwTicksPerSec));
}

float CTimer::GetLoopTime()
{
  return mLoopTime;
}

LARGE_INTEGER CTimer::GetTicks()
{
  LARGE_INTEGER performanceCount{};
  if (QueryPerformanceCounter(&performanceCount))
  {
    while (!performanceCount.QuadPart)
    {
      QueryPerformanceCounter(&performanceCount);
    }
    return performanceCount;
  }

  LARGE_INTEGER fallback{};
  fallback.QuadPart = static_cast<LONGLONG>(timeGetTime());
  return fallback;
}

float CTimer::GetTime()
{
  return mRealTime;
}

void CTimer::SetMinFPS(float fps)
{
  mMinFPS = 1.0f / fps;
}

void CTimer::SetTime(float timeValue)
{
  mRealTime = timeValue;
}

