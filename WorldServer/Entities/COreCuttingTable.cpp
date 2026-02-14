#include "pch.h"

#include "COreCuttingTable.h"

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

  pOreList = new _ore_cut_list[m_nOreNum]();
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
      _base_fld *cuttingRecord = m_tblOreCutting.GetRecord(cuttingIndex);
      if (strcmp_0(cuttingRecord->m_strCode, oreRecord->m_strCode) != 0)
      {
        continue;
      }

      unsigned __int16 *resourceIndex =
        reinterpret_cast<unsigned __int16 *>(resTable->GetRecord(reinterpret_cast<const char *>(&cuttingRecord[1])));
      if (!resourceIndex)
      {
        if (errCode)
        {
          std::sprintf(errCode, "COreCuttingTable.. %s no search index", reinterpret_cast<char *>(&cuttingRecord[1]));
        }
        return false;
      }

      _ore_cut_list &oreList = pOreList[oreIndex];
      oreList.ResList[resultCount].wResIndex = *resourceIndex;
      oreList.ResList[resultCount].dwRate = *reinterpret_cast<unsigned int *>(&cuttingRecord[1].m_strCode[60]);
      cumulativeRate += oreList.ResList[resultCount].dwRate;
      oreList.ResList[resultCount].dwCumRate = cumulativeRate;
      ++resultCount;
    }

    pOreList[oreIndex].nResNum = resultCount;
    pOreList[oreIndex].dwTotalRate = cumulativeRate;
  }

  return true;
}
