#include "pch.h"

#include "COreCuttingTable.h"
#include "OreCutting_fld.h"

COreCuttingTable::COreCuttingTable()
  : m_tblOreCutting(), m_nOreNum(0), pOreList(nullptr)
{
}

COreCuttingTable::_ore_cut_list::_ore_cut_list()
  : nResNum(0),
    dwTotalRate(0)
{
}

COreCuttingTable::~COreCuttingTable()
{
  if (pOreList)
  {
    delete[] pOreList;
    pOreList = nullptr;
  }
}

bool COreCuttingTable::ReadRecord(const char *fileName, CRecordData *oreTable, CRecordData *resTable, char *errCode)
{
  if (!m_tblOreCutting.ReadRecord(fileName, 0x88, errCode))
  {
    return false;
  }

  m_nOreNum = oreTable->GetRecordNum();
  if (m_nOreNum <= 0)
  {
    if (errCode)
    {
      std::sprintf(errCode, "COreCuttingTable.. ore rec num <= 0");
    }
    return false;
  }

  pOreList = new _ore_cut_list[m_nOreNum];
  return Indexing(oreTable, resTable, errCode);
}

unsigned int COreCuttingTable::GetOreIndexFromRate(unsigned int dwOreIndex, unsigned int dwRate)
{
  if (!pOreList || dwOreIndex >= static_cast<unsigned int>(m_nOreNum))
  {
    return static_cast<unsigned int>(-1);
  }

  _ore_cut_list *oreList = &pOreList[dwOreIndex];
  for (int index = 0; index < oreList->nResNum; ++index)
  {
    if (dwRate < oreList->ResList[index].dwCumRate)
    {
      return oreList->ResList[index].wResIndex;
    }
  }

  if (oreList->nResNum > 0)
  {
    return oreList->ResList[oreList->nResNum - 1].wResIndex;
  }

  return static_cast<unsigned int>(-1);
}

bool COreCuttingTable::Indexing(CRecordData *oreTable, CRecordData *resTable, char *errCode)
{
  for (int oreRecordIndex = 0; oreRecordIndex < m_nOreNum; ++oreRecordIndex)
  {
    _base_fld *oreRecord = oreTable->GetRecord(oreRecordIndex);
    const unsigned int oreIndex = oreRecord->m_dwIndex;

    int resultCount = 0;
    unsigned int cumulativeRate = 0;

    const int cuttingRecordCount = m_tblOreCutting.GetRecordNum();
    for (int cuttingIndex = 0; cuttingIndex < cuttingRecordCount; ++cuttingIndex)
    {
      _OreCutting_fld *cuttingRecord =
        reinterpret_cast<_OreCutting_fld *>(m_tblOreCutting.GetRecord(cuttingIndex));
      if (strcmp_0(cuttingRecord->m_strCode, oreRecord->m_strCode) != 0)
      {
        continue;
      }

      _base_fld *resourceRecord = resTable->GetRecord(cuttingRecord->m_strResource_Item);
      if (!resourceRecord)
      {
        if (errCode)
        {
          std::sprintf(errCode, "COreCuttingTable.. %s no search index", cuttingRecord->m_strResource_Item);
        }
        return false;
      }

      _ore_cut_list &oreList = pOreList[oreIndex];
      oreList.ResList[resultCount].wResIndex = static_cast<unsigned __int16>(resourceRecord->m_dwIndex);
      oreList.ResList[resultCount].dwRate = static_cast<unsigned int>(cuttingRecord->m_nCutting_probability);
      cumulativeRate += oreList.ResList[resultCount].dwRate;
      oreList.ResList[resultCount].dwCumRate = cumulativeRate;
      ++resultCount;
    }

    pOreList[oreIndex].nResNum = resultCount;
    pOreList[oreIndex].dwTotalRate = cumulativeRate;
  }

  return true;
}
