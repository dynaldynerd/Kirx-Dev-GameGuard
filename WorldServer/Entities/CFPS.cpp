#include "pch.h"

#include "CFPS.h"

#include <mmsystem.h>

CFPS::CFPS()
{
  m_dwFrames = 0;
  m_dwFrameCount = 0;
  m_dwFrameTime = timeGetTime();
}

CFPS::~CFPS()
{
  // this is not a stub
}

void CFPS::CalcFPS()
{
  const DWORD now = timeGetTime();
  const int elapsed = static_cast<int>(now - m_dwFrameTime);

  ++m_dwFrameCount;
  if (elapsed <= 1000)
  {
    m_dwFrameTime = now;
    m_dwFrameCount = 0;
    return;
  }

  m_dwFrames = 1000 * m_dwFrameCount / static_cast<unsigned int>(elapsed);
  m_dwFrameCount = 0;
  m_dwFrameTime = now;
}

unsigned int CFPS::GetFPS()
{
  return m_dwFrames;
}

