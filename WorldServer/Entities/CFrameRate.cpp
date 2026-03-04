#include "pch.h"

#include "CFrameRate.h"

#include <mmsystem.h>

CFrameRate::CFrameRate()
  : m_fSpeedPerFrame(1.0f),
    m_dwFrames(0),
    m_dwFrameTime(0),
    m_dwFrameCount(0),
    m_dwDisplayTick(0)
{
}

CFrameRate::~CFrameRate()
{
  // this is not a stub
}

void CFrameRate::CalcSpeedPerFrame()
{
  const DWORD now = timeGetTime();
  const int delta = static_cast<int>(now - m_dwFrameTime);
  ++m_dwFrameCount;
  if (delta < 1000)
  {
    if (delta < 0)
    {
      m_dwFrameTime = now;
      m_dwFrameCount = 0;
    }
    return;
  }

  m_dwFrames = static_cast<unsigned int>(1000 * m_dwFrameCount / delta);
  m_dwFrameCount = 0;
  m_dwFrameTime = now;
  m_fSpeedPerFrame = static_cast<float>(static_cast<int>(m_dwFrames)) / 15.0f;
  m_fSpeedPerFrame = 1.0f / m_fSpeedPerFrame;
  if (m_fSpeedPerFrame > 1.0f)
  {
    m_fSpeedPerFrame = 1.0f;
  }
}

float CFrameRate::GetSpeedPerFrame()
{
  return m_fSpeedPerFrame;
}

unsigned int CFrameRate::GetFPS()
{
  return m_dwFrames;
}

