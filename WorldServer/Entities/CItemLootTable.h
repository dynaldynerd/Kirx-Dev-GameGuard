#pragma once

#include "IdaCompat.h"
#include "CRecordData.h"

class CEventLootTable;

/* 1284 */
class  CItemLootTable
{
public:
  CItemLootTable();

  struct _linker_code
  {
    unsigned __int8 byTableCode;
    unsigned __int16 wItemIndex;
    int bExist;
  };

  bool ReadRecord(const char *fileName, CRecordData *itemTables, char *errCode);

  virtual ~CItemLootTable();

private:
  bool Indexing(CRecordData *itemTables, char *errCode);
public:
  CRecordData m_tblLoot;
  CEventLootTable *m_pTblEvent;
  _linker_code **m_ppLinkCode;
  int m_nLootNum;
};


