#pragma once

#include "IdaCompat.h"

/* 1267 */
class __cppobj CMyCriticalSection
{
public:
  CMyCriticalSection();
  void Lock();
  void Unlock();
  virtual ~CMyCriticalSection();

  _RTL_CRITICAL_SECTION m_cs;
};

