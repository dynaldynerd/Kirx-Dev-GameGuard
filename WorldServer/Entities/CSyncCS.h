#pragma once

#include "IdaCompat.h"

/* 1692 */
struct __cppobj CSyncCS
{
  bool m_bUse;
  _RTL_CRITICAL_SECTION m_cs;
  virtual ~CSyncCS() = default;
};
