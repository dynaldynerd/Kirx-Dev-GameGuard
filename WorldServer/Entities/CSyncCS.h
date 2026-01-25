#pragma once

#include "IdaCompat.h"

/* 1692 */
class __cppobj CSyncCS
{
public:
  bool m_bUse;
  _RTL_CRITICAL_SECTION m_cs;

  static void SetUse(CSyncCS *cs, bool use);
  static void Lock(CSyncCS *cs);
  static void Unlock(CSyncCS *cs);
  virtual ~CSyncCS();
};

