#include "pch.h"

#include "COreCuttingTable.h"

bool COreCuttingTable::ReadRecord(const char *fileName, CRecordData *oreTable, CRecordData *resTable, char *errCode)
{
  (void)fileName;
  (void)oreTable;
  (void)resTable;
  if (errCode != nullptr)
  {
    errCode[0] = '\0';
  }
  return true;
}
