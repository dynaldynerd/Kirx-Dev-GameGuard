#pragma once

#include "IdaCompat.h"

/* 1776 */
class  CAITimer
{
public:
  CAITimer();
  void Init(unsigned int delay);
  void Set(unsigned int delay);
  int Check();

private:
  unsigned int m_BefTime;
  unsigned int m_Delay;
  unsigned int m_DDelay;
};

