#include "pch.h"

#include "CMapDataTable.h"

#include <cstdio>
#include <cstring>
#include "map_fld.h"

CMapDataTable::CMapDataTable()
{
  m_pRecord = nullptr;
  m_dwRecordNum = 0;
}

CMapDataTable::~CMapDataTable()
{
  if (m_pRecord)
  {
    delete[] m_pRecord;
    m_pRecord = nullptr;
  }
}

bool CMapDataTable::ReadScript(const char *szFileName)
{
  if (this->m_pRecord)
    return false;

  FILE *Stream = nullptr;
  if (fopen_s(&Stream, szFileName, "rt") != 0 || !Stream)
    return false;

  char Str1[132];
  unsigned int v8 = 0;
  while (fscanf_s(Stream, "%s", Str1, (unsigned int)sizeof(Str1)) != -1)
  {
    if (std::strcmp(Str1, "*") == 0)
      ++v8;
  }

  this->m_dwRecordNum = v8;
  this->m_pRecord = new _map_fld[v8];

  std::rewind(Stream);
  unsigned int v9 = 0;
  char v10[16];
  while (fscanf_s(Stream, "%s", Str1, (unsigned int)sizeof(Str1)) != -1)
  {
    if (std::strcmp(Str1, "*") == 0)
    {
      fscanf_s(Stream, "%s", this->m_pRecord[v9].m_strCode, (unsigned int)sizeof(this->m_pRecord[v9].m_strCode));
      fscanf_s(Stream, "%s", this->m_pRecord[v9].m_strFileName, (unsigned int)sizeof(this->m_pRecord[v9].m_strFileName));
      fscanf_s(Stream, "%d", &this->m_pRecord[v9].m_nMapType);
      fscanf_s(Stream, "%d", &this->m_pRecord[v9].m_nMapClass);
      fscanf_s(Stream, "%d", &this->m_pRecord[v9].m_nLayerNum);
      fscanf_s(Stream, "%d", &this->m_pRecord[v9].m_nRaceVillageCode);
      fscanf_s(Stream, "%d", &this->m_pRecord[v9].m_nMonsterSetFileNum);
      fscanf_s(Stream, "%d", &this->m_pRecord[v9].m_nRadius);
      fscanf_s(Stream, "%d", &this->m_pRecord[v9].m_nLevelLimit);
      fscanf_s(Stream, "%d", &this->m_pRecord[v9].m_nUpLevelLim);
      fscanf_s(Stream, "%d", &this->m_pRecord[v9].m_nPotionLim);
      fscanf_s(Stream, "%s", v10, (unsigned int)sizeof(v10));
      for (int j = 0; j < 3; ++j)
        this->m_pRecord[v9].m_nRacePvpUsable[j] = (char)v10[j] - '0';
      this->m_pRecord[v9].m_dwIndex = v9;
      ++v9;
    }
  }
  std::fclose(Stream);
  return true;
}

int CMapDataTable::GetRecordNum()
{
  return (int)m_dwRecordNum;
}

_map_fld *CMapDataTable::GetRecord(unsigned int dwIndex)
{
  if (dwIndex < m_dwRecordNum)
  {
    return &m_pRecord[dwIndex];
  }
  return nullptr;
}

