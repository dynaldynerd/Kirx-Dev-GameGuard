#pragma once

#include "IdaCompat.h"

class __cppobj __declspec(align(8)) CMyTimer
{
public:
  CMyTimer();
  void BeginTimer(unsigned int tickTerm);
  void BeginTimerAddLapse(int dwTerm, unsigned int dwAddLapse);
  bool CountingTimer();
  void TermTimeRun();
  void StopTimer();
  virtual ~CMyTimer();

  int m_nTickTerm;
  unsigned int m_dwTickOld;
  bool m_bOper;
};
