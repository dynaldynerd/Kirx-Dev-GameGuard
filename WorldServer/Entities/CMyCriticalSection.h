#pragma once

#include "IdaCompat.h"

/* 1267 */
class  CMyCriticalSection
{
public:
  CMyCriticalSection();
  void Lock();
  void Unlock();
  virtual ~CMyCriticalSection();

  _RTL_CRITICAL_SECTION m_cs;
};

