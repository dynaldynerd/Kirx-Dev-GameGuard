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
