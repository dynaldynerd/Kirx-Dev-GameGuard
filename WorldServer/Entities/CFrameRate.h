#pragma once

#include "IdaCompat.h"

/* 1262 */
class  CFrameRate
{
public:
  CFrameRate();
  float m_fSpeedPerFrame;
  unsigned int m_dwFrames;
  unsigned int m_dwFrameTime;
  unsigned int m_dwFrameCount;
  unsigned int m_dwDisplayTick;
  virtual ~CFrameRate();

  void CalcSpeedPerFrame();
  float GetSpeedPerFrame();
  unsigned int GetFPS();
};


