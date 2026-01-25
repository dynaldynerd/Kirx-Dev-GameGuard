#include "pch.h"

#include "CItemLootTable.h"

bool CItemLootTable::ReadRecord(const char *fileName, CRecordData *itemTables, char *errCode)
{
  (void)fileName;
  (void)itemTables;
  if (errCode != nullptr)
  {
    errCode[0] = '\0';
  }
  return true;
}
