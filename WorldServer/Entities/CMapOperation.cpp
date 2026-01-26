#include "pch.h"

#include "CMapOperation.h"

#include <cstring>

CMapOperation g_MapOper;

bool CMapOperation::Init()
{
  return true;
}

CMapData *CMapOperation::GetMap(char *szMapCode)
{
  int scratch[12]{};
  std::memset(scratch, 0xCC, sizeof(scratch));

  for (int j = 0; j < m_nMapNum; ++j)
  {
    if (!std::strcmp(m_Map[j].m_pMapSet->m_strCode, szMapCode))
    {
      return &m_Map[j];
    }
  }
  return nullptr;
}
