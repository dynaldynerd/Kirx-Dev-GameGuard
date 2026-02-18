#pragma once

#include "IdaCompat.h"

/* 1131 */
class  CNetCriticalSection
{
public:
  CNetCriticalSection();
  ~CNetCriticalSection();

  void Lock();
  void Unlock();

  _RTL_CRITICAL_SECTION m_cs;
};

