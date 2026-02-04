#include "pch.h"

#include "CItemUpgradeTable.h"

#include "CRecordData.h"
#include "ItemUpgrade_fld.h"

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

_ItemUpgrade_fld *CItemUpgradeTable::GetRecord(unsigned int dwIndex)
{
  if (dwIndex < static_cast<unsigned int>(m_nResNum))
  {
    return reinterpret_cast<_ItemUpgrade_fld *>(m_tblItemUpgrade.GetRecord(dwIndex));
  }
  return nullptr;
}
