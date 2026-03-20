#pragma once

#include "IdaCompat.h"

class  C24Timer
{
public:
  unsigned int m_dwBaseTickTime;
  unsigned int m_dwBase24Time;

  C24Timer();
  ~C24Timer();

  void Init();
  unsigned int Get24TimeFromTickTime(unsigned int dwTickTime);
};

