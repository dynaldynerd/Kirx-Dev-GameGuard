#include "pch.h"

#include "CItemUpgradeTable.h"

#include "CRecordData.h"
#include "ItemUpgrade_fld.h"

CItemUpgradeTable::CItemUpgradeTable()
  : m_tblItemUpgrade(), m_nResNum(0), m_pwResIndex(nullptr)
{
}

CItemUpgradeTable::~CItemUpgradeTable()
{
  if (m_pwResIndex != nullptr)
  {
    delete[] m_pwResIndex;
    m_pwResIndex = nullptr;
  }
}

bool CItemUpgradeTable::ReadRecord(const char *fileName, CRecordData *resTable, char *errCode)
{
  if (!m_tblItemUpgrade.ReadRecord(fileName, 216, errCode))
  {
    return false;
  }

  m_nResNum = m_tblItemUpgrade.GetRecordNum();
  if (m_nResNum <= 0)
  {
    if (errCode)
    {
      std::sprintf(errCode, "CItemUpgradeTable.. record Num <= 0");
    }
    return false;
  }

  m_pwResIndex = new unsigned __int16[m_nResNum];
  return Indexing(resTable, errCode);
}

_ItemUpgrade_fld *CItemUpgradeTable::GetRecord(unsigned int dwIndex)
{
  if (dwIndex < static_cast<unsigned int>(m_nResNum))
  {
    return reinterpret_cast<_ItemUpgrade_fld *>(m_tblItemUpgrade.GetRecord(dwIndex));
  }
  return nullptr;
}

_ItemUpgrade_fld *CItemUpgradeTable::GetRecordFromRes(unsigned int dwResIndex)
{
  for (int n = 0; n < m_nResNum; ++n)
  {
    if (m_pwResIndex[n] == dwResIndex)
    {
      return reinterpret_cast<_ItemUpgrade_fld *>(m_tblItemUpgrade.GetRecord(n));
    }
  }
  return nullptr;
}

unsigned int CItemUpgradeTable::GetSize()
{
  return static_cast<unsigned int>(m_nResNum);
}

bool CItemUpgradeTable::Indexing(CRecordData *resTable, char *errCode)
{
  for (int index = 0; index < m_nResNum; ++index)
  {
    _base_fld *upgradeRecord = m_tblItemUpgrade.GetRecord(index);
    _base_fld *resourceRecord = resTable->GetRecord(upgradeRecord->m_strCode);
    if (!resourceRecord)
    {
      if (errCode)
      {
        std::sprintf(errCode, "CItemUpgradeTable.. %d rec %s data no search index", index, upgradeRecord->m_strCode);
      }
      return false;
    }

    m_pwResIndex[index] = static_cast<unsigned __int16>(resourceRecord->m_dwIndex);
  }

  return true;
}
