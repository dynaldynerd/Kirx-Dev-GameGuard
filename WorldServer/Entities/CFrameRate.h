#pragma once

#include "IdaCompat.h"

/* 1262 */
class __cppobj __declspec(align(8)) CFrameRate
{
public:
  float m_fSpeedPerFrame;
  unsigned int m_dwFrames;
  unsigned int m_dwFrameTime;
  unsigned int m_dwFrameCount;
  unsigned int m_dwDisplayTick;
  virtual ~CFrameRate() = default;
};

