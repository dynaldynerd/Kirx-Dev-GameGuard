#pragma once

#include "IdaCompat.h"

/* 1692 */
class __cppobj CSyncCS
{
public:
  bool m_bUse;
  _RTL_CRITICAL_SECTION m_cs;

  void SetUse(bool use);
  void Lock();
  void Unlock();
  virtual ~CSyncCS();
};

