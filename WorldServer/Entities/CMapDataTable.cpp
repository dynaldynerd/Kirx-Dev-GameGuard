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

  char token[132];
  unsigned int recordCount = 0;
  while (fscanf_s(Stream, "%s", token, (unsigned int)sizeof(token)) != -1)
  {
    if (std::strcmp(token, "*") == 0)
      ++recordCount;
  }

  this->m_dwRecordNum = recordCount;
  this->m_pRecord = new _map_fld[recordCount];

  std::rewind(Stream);
  unsigned int recordIndex = 0;
  char racePvpFlags[16];
  while (fscanf_s(Stream, "%s", token, (unsigned int)sizeof(token)) != -1)
  {
    if (std::strcmp(token, "*") == 0)
    {
      fscanf_s(Stream, "%s", this->m_pRecord[recordIndex].m_strCode, (unsigned int)sizeof(this->m_pRecord[recordIndex].m_strCode));
      fscanf_s(Stream, "%s", this->m_pRecord[recordIndex].m_strFileName, (unsigned int)sizeof(this->m_pRecord[recordIndex].m_strFileName));
      fscanf_s(Stream, "%d", &this->m_pRecord[recordIndex].m_nMapType);
      fscanf_s(Stream, "%d", &this->m_pRecord[recordIndex].m_nMapClass);
      fscanf_s(Stream, "%d", &this->m_pRecord[recordIndex].m_nLayerNum);
      fscanf_s(Stream, "%d", &this->m_pRecord[recordIndex].m_nRaceVillageCode);
      fscanf_s(Stream, "%d", &this->m_pRecord[recordIndex].m_nMonsterSetFileNum);
      fscanf_s(Stream, "%d", &this->m_pRecord[recordIndex].m_nRadius);
      fscanf_s(Stream, "%d", &this->m_pRecord[recordIndex].m_nLevelLimit);
      fscanf_s(Stream, "%d", &this->m_pRecord[recordIndex].m_nUpLevelLim);
      fscanf_s(Stream, "%d", &this->m_pRecord[recordIndex].m_nPotionLim);
      fscanf_s(Stream, "%s", racePvpFlags, (unsigned int)sizeof(racePvpFlags));
      for (int j = 0; j < 3; ++j)
        this->m_pRecord[recordIndex].m_nRacePvpUsable[j] = (char)racePvpFlags[j] - '0';
      this->m_pRecord[recordIndex].m_dwIndex = recordIndex;
      ++recordIndex;
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

