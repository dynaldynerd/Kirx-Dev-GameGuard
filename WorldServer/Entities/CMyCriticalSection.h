#pragma once

#include "IdaCompat.h"

/* 1267 */
class __cppobj CMyCriticalSection
{
  _RTL_CRITICAL_SECTION m_cs;
  virtual ~CMyCriticalSection() = default;
};

