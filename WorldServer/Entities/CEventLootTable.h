#pragma once

#include "IdaCompat.h"

/* 1286 */
class __cppobj CEventLootTable
{
  struct _event_drop
  {
    char strCode[64];
    unsigned __int16 wMagnifications;
    unsigned __int16 wRange;
    unsigned __int16 wDropCntOnce;
    unsigned __int16 wDropDelay;
    _event_drop *pNext;
  };

  _event_drop *m_pEventDropList;
  virtual ~CEventLootTable() = default;
};

