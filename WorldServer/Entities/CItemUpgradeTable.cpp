#include "pch.h"

#include "CItemUpgradeTable.h"

bool CItemUpgradeTable::ReadRecord(const char *fileName, CRecordData *resTable, char *errCode)
{
  (void)fileName;
  (void)resTable;
  if (errCode != nullptr)
  {
    errCode[0] = '\0';
  }
  return true;
}
