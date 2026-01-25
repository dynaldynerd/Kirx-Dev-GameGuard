#pragma once

#include "IdaCompat.h"

/* 1267 */
class __cppobj CMyCriticalSection
{
public:
  _RTL_CRITICAL_SECTION m_cs;
  virtual ~CMyCriticalSection() = default;
};

