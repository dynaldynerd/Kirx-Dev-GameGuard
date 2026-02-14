#pragma once

#include "IdaCompat.h"

/* 1284 */
class __cppobj __declspec(align(8)) CItemLootTable
{
public:
  struct _linker_code
  {
    unsigned __int8 byTableCode;
    unsigned __int16 wItemIndex;
    int bExist;
  };

  bool ReadRecord(const char *fileName, CRecordData *itemTables, char *errCode);

  virtual ~CItemLootTable() = default;

private:
  bool Indexing(CRecordData *itemTables, char *errCode);
public:
  CRecordData m_tblLoot;
  CEventLootTable *m_pTblEvent;
  _linker_code **m_ppLinkCode;
  int m_nLootNum;
};

