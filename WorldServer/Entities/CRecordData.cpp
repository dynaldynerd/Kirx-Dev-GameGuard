#include "pch.h"

#include "CRecordData.h"

#include <cstring>

bool CRecordData::ReadRecord(const char *fileName, int structSize, char *errCode)
{
  (void)structSize;
  if (fileName == nullptr)
  {
    if (errCode != nullptr)
    {
      errCode[0] = '\0';
    }
    return false;
  }
  strncpy_s(m_szFileName, sizeof(m_szFileName), fileName, _TRUNCATE);
  if (errCode != nullptr)
  {
    errCode[0] = '\0';
  }
  m_bLoad = true;
  return true;
}

unsigned int CRecordData::GetRecordNum() const
{
  return m_dwTotalSize;
}

_base_fld *CRecordData::GetRecord(int index)
{
  (void)index;
  return m_bLoad ? reinterpret_cast<_base_fld *>(this) : nullptr;
}

_base_fld *CRecordData::GetRecord(const char *code)
{
  (void)code;
  return m_bLoad ? reinterpret_cast<_base_fld *>(this) : nullptr;
}

bool CRecordData::MakeHashTable(int keyIndex, int keyLength, char *errCode)
{
  if (!m_bLoad)
  {
    if (errCode != nullptr)
    {
      strcpy_s(errCode, 256, "A");
    }
    return false;
  }

  if (m_pdwHashList != nullptr)
  {
    if (errCode != nullptr)
    {
      strcpy_s(errCode, 256, "AI");
    }
    return false;
  }

  if (m_Header.m_nRecordNum <= 0)
  {
    if (errCode != nullptr)
    {
      errCode[0] = '\0';
    }
    return true;
  }

  m_pdwHashList = new unsigned int[static_cast<unsigned int>(m_Header.m_nRecordNum)];
  for (int n = 0; n < m_Header.m_nRecordNum; ++n)
  {
    _base_fld *record = GetRecord(n);
    m_pdwHashList[n] = record == nullptr ? 0 : MakeHash(&record->m_strCode[keyIndex], keyLength);
  }

  return true;
}

unsigned int CRecordData::MakeHash(const char *p, int len)
{
  static unsigned char s_hashTable[256] = {};
  static bool s_hashTableLoaded = false;

  if (!s_hashTableLoaded)
  {
    for (int j = 0; j < 26; ++j)
    {
      s_hashTable[j + 'a'] = static_cast<unsigned char>(j);
      s_hashTable[j + 'A'] = static_cast<unsigned char>(j);
    }
    for (int j = 0; j < 10; ++j)
    {
      s_hashTable[j + '0'] = static_cast<unsigned char>(j + 26);
    }
    s_hashTableLoaded = true;
  }

  unsigned int hash = 0;
  for (int k = 0; k < len; ++k)
  {
    unsigned int v = s_hashTable[static_cast<unsigned char>(*p++)];
    hash |= v << (6 * k);
  }
  return hash;
}
