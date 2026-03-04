#pragma once

#include "IdaCompat.h"

/* 1692 */
class  CSyncCS
{
public:
  CSyncCS();

  bool m_bUse;
  _RTL_CRITICAL_SECTION m_cs;

  bool IsUse();
  void SetUse(bool use);
  void Lock();
  void Unlock();
  virtual ~CSyncCS();
};

