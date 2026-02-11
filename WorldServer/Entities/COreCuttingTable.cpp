#include "pch.h"

#include "COreCuttingTable.h"

#include <cstdio>
#include <new>

#include "CRecordData.h"

COreCuttingTable::_ore_cut_list::_ore_cut_list()
{
  nResNum = 0;
  dwTotalRate = 0;
}

COreCuttingTable::COreCuttingTable()
  : m_nOreNum(0),
    pOreList(nullptr)
{
}

COreCuttingTable::~COreCuttingTable()
{
  if (pOreList)
  {
    operator delete[](pOreList);
    pOreList = nullptr;
  }
}

bool COreCuttingTable::ReadRecord(const char *fileName, CRecordData *oreTable, CRecordData *resTable, char *errCode)
{
  if (!m_tblOreCutting.ReadRecord(fileName, 0x88u, errCode))
  {
    return false;
  }

  m_nOreNum = oreTable->GetRecordNum();
  if (m_nOreNum > 0)
  {
    const size_t count = static_cast<size_t>(m_nOreNum);
    void *mem = operator new[](saturated_mul(count, 0x4B8uLL));
    _ore_cut_list *list = nullptr;
    if (mem)
    {
      list = static_cast<_ore_cut_list *>(mem);
      for (size_t i = 0; i < count; ++i)
      {
        new (&list[i]) _ore_cut_list();
      }
    }
    pOreList = list;
    return Indexing(oreTable, resTable, errCode);
  }

  if (errCode)
  {
    sprintf(errCode, "CoreCuttingTable: ore record num <= 0");
  }
  return false;
}

char COreCuttingTable::Indexing(CRecordData *pOreRec, CRecordData *pResRec, char *pszErrMsg)
{
  for (int n = 0; n < m_nOreNum; ++n)
  {
    _base_fld *oreRecord = pOreRec->GetRecord(n);
    const unsigned int oreIndex = oreRecord->m_dwIndex;
    int resNum = 0;
    unsigned int totalRate = 0;

    const int recordNum = m_tblOreCutting.GetRecordNum();
    for (int j = 0; j < recordNum; ++j)
    {
      _base_fld *cutRecord = m_tblOreCutting.GetRecord(j);
      if (strcmp_0(cutRecord->m_strCode, oreRecord->m_strCode))
      {
        continue;
      }

      unsigned __int16 *resIndex = reinterpret_cast<unsigned __int16 *>(
        pResRec->GetRecord(reinterpret_cast<const char *>(&cutRecord[1])));
      if (resIndex)
      {
        pOreList[oreIndex].ResList[resNum].wResIndex = *resIndex;
        pOreList[oreIndex].ResList[resNum].dwRate =
          *reinterpret_cast<unsigned int *>(&cutRecord[1].m_strCode[60]);
        totalRate += *reinterpret_cast<unsigned int *>(&cutRecord[1].m_strCode[60]);
        pOreList[oreIndex].ResList[resNum++].dwCumRate = totalRate;
      }
      else
      {
        if (pszErrMsg)
        {
          sprintf(pszErrMsg, "CoreCuttingTable: res record not found (%s)", reinterpret_cast<char *>(&cutRecord[1]));
        }
        return 0;
      }
    }
    pOreList[oreIndex].nResNum = resNum;
    pOreList[oreIndex].dwTotalRate = totalRate;
  }
  return 1;
}

int COreCuttingTable::GetOreIndexFromRate(unsigned int dwOreIndex, unsigned int dwRate)
{
  for (unsigned int j = 0; j < static_cast<unsigned int>(pOreList[dwOreIndex].nResNum); ++j)
  {
    if (dwRate < pOreList[dwOreIndex].ResList[j].dwCumRate)
    {
      return pOreList[dwOreIndex].ResList[j].wResIndex;
    }
  }

  if (pOreList[dwOreIndex].nResNum)
  {
    const unsigned int idx = static_cast<unsigned int>(pOreList[dwOreIndex].nResNum) + 99;
    return static_cast<unsigned __int16>(pOreList[dwOreIndex - 1].ResList[idx].dwCumRate);
  }

  return -1;
}
