#pragma once

#include "IdaCompat.h"

/* 1428 */
class  CTimer
{
  float mLoopTime;
  float mTime;
  float mRealTime;
  float mMinFPS;
  float mfLoopHop;
  unsigned int mOldTime;
  unsigned int mLoopCnt;
  unsigned int mLoopHop;
  int m_bUsingQPF;
  int m_bTimerInitialized;
  float mFPS;
  unsigned int mLoopFPSCnt;
  float mFPSTime;
  float m_fTicksPerSec;
  float m_fFramesPerSec;
  float m_fAverageFramesPerSec;
  float m_fSecsPerFrame;
  float m_fLamTime;
  unsigned __int64 m_qwTicks;
  unsigned __int64 m_qwStartTicks;
  unsigned __int64 m_qwTicksPerSec;
  unsigned __int64 m_qwTicksPerFrame;
};

