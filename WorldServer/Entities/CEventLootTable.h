#pragma once

#include "IdaCompat.h"

/* 1286 */
class __cppobj CEventLootTable
{
public:
  struct _event_drop
  {
    char strCode[64];
    unsigned __int16 wMagnifications;
    unsigned __int16 wRange;
    unsigned __int16 wDropCntOnce;
    unsigned __int16 wDropDelay;
    _event_drop *pNext;

    _event_drop();
  };

  CEventLootTable();
  bool ReadRecord();
  void AddRecord(_event_drop *pEventDrop);
  _event_drop *GetRecord(const char *szRecordCode);
  virtual ~CEventLootTable();

private:
  _event_drop *m_pEventDropList;
};

