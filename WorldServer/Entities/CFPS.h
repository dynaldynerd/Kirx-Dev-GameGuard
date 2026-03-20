#pragma once

#include "IdaCompat.h"

class  CFPS
{
public:
  CFPS();
  virtual ~CFPS();

  void CalcFPS();
  unsigned int GetFPS();

private:
  unsigned int m_dwFrames;
  unsigned int m_dwFrameTime;
  unsigned int m_dwFrameCount;
};

