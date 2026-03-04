#pragma once

#include "IdaCompat.h"

class  CMyTimer
{
public:
  CMyTimer();
  void BeginTimer(unsigned int tickTerm);
  void BeginTimerAddLapse(int dwTerm, unsigned int dwAddLapse);
  void CountingAddTickOld(unsigned int dwAddGap);
  bool CountingTimer();
  unsigned int GetTerm();
  void TermTimeRun();
  void NextTimeRun();
  void StopTimer();
  virtual ~CMyTimer();

  int m_nTickTerm;
  unsigned int m_dwTickOld;
  bool m_bOper;
};

