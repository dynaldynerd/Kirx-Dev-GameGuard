#pragma once

#include "IdaCompat.h"

class __cppobj CFPS
{
public:
  CFPS();
  virtual ~CFPS();

  void CalcFPS();
  __int64 GetFPS();

private:
  unsigned int m_dwFrames;
  unsigned int m_dwFrameTime;
  unsigned int m_dwFrameCount;
};

