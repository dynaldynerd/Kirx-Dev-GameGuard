#pragma once

#include "IdaCompat.h"

class __cppobj __declspec(align(8)) CMyTimer
{
public:
  void BeginTimer(unsigned int tickTerm);
  virtual ~CMyTimer();

  int m_nTickTerm;
  unsigned int m_dwTickOld;
  bool m_bOper;
};
