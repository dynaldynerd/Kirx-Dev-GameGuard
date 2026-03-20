#include "pch.h"

#include "base_fld.h"

#include "CRecordData.h"

#include <cstring>
#include <cstdio>

namespace
{
  static constexpr size_t kErrCodeSize = 152;
}

CRecordData::CRecordData()
{
  m_bLoad = false;
  std::memset(m_szFileName, 0, sizeof(m_szFileName));
  m_dwTotalSize = 0;
  std::memset(&m_Header, 0, sizeof(m_Header));
  m_nLowNum = 0;
  m_ppsRecord = nullptr;
  m_pdwHashList = nullptr;
}

bool CRecordData::ReadRecord(const char *fileName, int structSize, char *errCode)
{
  if (fileName == nullptr)
  {
    if (errCode != nullptr)
    {
      errCode[0] = '\0';
    }
    return false;
  }

  if (std::strlen(fileName) > 128)
  {
    return false;
  }

  if (m_bLoad)
  {
    if (std::strcmp(m_szFileName, fileName) != 0)
    {
      return false;
    }
  }
  else
  {
    std::strcpy(m_szFileName, fileName);
  }

  bool ok = true;
  HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    ok = false;
  }
  else if (LoadRecordHeader(hFile, errCode))
  {
    if (!LoadRecordData(hFile, errCode))
    {
      ok = false;
    }
  }
  else
  {
    ok = false;
  }

  if (hFile != INVALID_HANDLE_VALUE)
  {
    CloseHandle(hFile);
  }

  if (ok)
  {
    const unsigned int fileSize = FileSize(fileName);
    if (m_dwTotalSize == fileSize)
    {
      if (static_cast<unsigned int>(structSize) == static_cast<unsigned int>(m_Header.m_nRecordSize))
      {
        m_bLoad = true;
        return true;
      }
      if (errCode != nullptr)
      {
        sprintf_s(
          errCode,
          kErrCodeSize,
          "%s : record size(%u) != struct size(%u)",
          fileName,
          static_cast<unsigned int>(m_Header.m_nRecordSize),
          static_cast<unsigned int>(structSize));
      }
      return false;
    }
    if (errCode != nullptr)
    {
      sprintf_s(
        errCode,
        kErrCodeSize,
        "%s : file size(%u) != real size(%u)",
        fileName,
        m_dwTotalSize,
        fileSize);
    }
    return false;
  }

  if (errCode != nullptr)
  {
    sprintf_s(errCode, kErrCodeSize, "%s : Access Denied!!", fileName);
  }
  return false;
}

bool CRecordData::ReadRecord_Ex(const char *fileName1, const char *fileName2, unsigned int structSize, char *errCode)
{
  CRecordData src;
  CRecordData extra;
  if (!src.ReadRecord(fileName1, static_cast<int>(structSize), errCode))
  {
    return false;
  }
  if (!extra.ReadRecord(fileName2, static_cast<int>(structSize), errCode))
  {
    return false;
  }

  m_Header = src.m_Header;
  m_Header.m_nRecordNum = static_cast<int>(src.GetRecordNum() + extra.GetRecordNum());

  const unsigned int recordCount = static_cast<unsigned int>(m_Header.m_nRecordNum);
  m_ppsRecord = new char *[recordCount];

  for (int j = 0; j < m_Header.m_nRecordNum; ++j)
  {
    m_ppsRecord[j] = new char[m_Header.m_nRecordSize];
  }

  int dst = 0;
  for (int j = 0; j < src.m_Header.m_nRecordNum; ++j)
  {
    std::memcpy(m_ppsRecord[dst++], src.m_ppsRecord[j], m_Header.m_nRecordSize);
  }
  for (int j = 0; j < extra.m_Header.m_nRecordNum; ++j)
  {
    std::memcpy(m_ppsRecord[dst], extra.m_ppsRecord[j], m_Header.m_nRecordSize);
    *reinterpret_cast<unsigned int *>(m_ppsRecord[dst]) = static_cast<unsigned int>(dst);
    ++dst;
  }

  m_bLoad = true;
  return true;
}

bool CRecordData::LoadRecordHeader(void *hFile, char *errCode)
{
  _record_bin_header buffer{};
  DWORD bytesRead = 0;
  ReadFile(hFile, &buffer, 4u, &bytesRead, nullptr);
  ReadFile(hFile, &buffer.m_nFieldNum, 4u, &bytesRead, nullptr);
  ReadFile(hFile, &buffer.m_nRecordSize, 4u, &bytesRead, nullptr);
  const unsigned int totalSize = buffer.m_nRecordSize * buffer.m_nRecordNum + 12;

  if (m_bLoad)
  {
    if (std::memcmp(&buffer, &m_Header, sizeof(m_Header)) != 0)
    {
      if (errCode != nullptr)
      {
        sprintf_s(
          errCode,
          kErrCodeSize,
          "%s : header mismatch (%u,%u,%u) != (%u,%u,%u)",
          m_szFileName,
          static_cast<unsigned int>(m_Header.m_nRecordNum),
          static_cast<unsigned int>(m_Header.m_nFieldNum),
          static_cast<unsigned int>(m_Header.m_nRecordSize),
          static_cast<unsigned int>(buffer.m_nRecordNum),
          static_cast<unsigned int>(buffer.m_nFieldNum),
          static_cast<unsigned int>(buffer.m_nRecordSize));
      }
      return false;
    }
    if (m_dwTotalSize != totalSize)
    {
      if (errCode != nullptr)
      {
        sprintf_s(
          errCode,
          kErrCodeSize,
          "%s : total size mismatch(%u) != (%u)",
          m_szFileName,
          m_dwTotalSize,
          totalSize);
      }
      return false;
    }
  }

  m_Header = buffer;
  m_dwTotalSize = totalSize;
  return true;
}

bool CRecordData::LoadRecordData(void *hFile, char *errCode)
{
  if (m_Header.m_nRecordNum && m_Header.m_nRecordSize)
  {
    m_nLowNum = m_Header.m_nRecordNum;
    const unsigned int recordCount = static_cast<unsigned int>(m_Header.m_nRecordNum);
    m_ppsRecord = new char *[recordCount];
    for (int j = 0; j < m_Header.m_nRecordNum; ++j)
    {
      m_ppsRecord[j] = new char[m_Header.m_nRecordSize];
      DWORD bytesRead = 0;
      if (m_bLoad)
      {
        char dest[80]{};
        std::strcpy(dest, m_ppsRecord[j] + 4);
        ReadFile(hFile, m_ppsRecord[j], m_Header.m_nRecordSize, &bytesRead, nullptr);
        if (std::strcmp(dest, m_ppsRecord[j] + 4) != 0)
        {
          if (errCode != nullptr)
          {
            sprintf_s(
              errCode,
              kErrCodeSize,
              "%s : record code mismatch %s != %s",
              m_szFileName,
              dest,
              m_ppsRecord[j] + 4);
          }
          return false;
        }
      }
      else
      {
        ReadFile(hFile, m_ppsRecord[j], m_Header.m_nRecordSize, &bytesRead, nullptr);
      }
    }
  }
  return true;
}

unsigned int CRecordData::FileSize(const char *fileName)
{
  HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    return 0;
  }
  const DWORD begin = SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
  const DWORD end = SetFilePointer(hFile, 0, nullptr, FILE_END);
  CloseHandle(hFile);
  return static_cast<unsigned int>(end - begin);
}

int CRecordData::GetRecordNum() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<unsigned int>(m_Header.m_nRecordNum));
}

_base_fld *CRecordData::GetRecord(int index)
{
  if (index >= 0 && index < m_Header.m_nRecordNum)
  {
    return reinterpret_cast<_base_fld *>(m_ppsRecord[index]);
  }
  return nullptr;
}

_base_fld *CRecordData::GetRecord(const char *code)
{
  for (int n = 0; n < m_Header.m_nRecordNum; ++n)
  {
    _base_fld *record = GetRecord(n);
    if (record != nullptr && std::strcmp(record->m_strCode, code) == 0)
    {
      return record;
    }
  }
  return nullptr;
}

_base_fld *CRecordData::GetRecord(const char *szRecordCode, int nCompareLen)
{
  for (int n = 0; n < m_Header.m_nRecordNum; ++n)
  {
    _base_fld *record = GetRecord(n);
    if (record != nullptr && std::strncmp(record->m_strCode, szRecordCode, nCompareLen) == 0)
    {
      return record;
    }
  }
  return nullptr;
}

_base_fld *CRecordData::GetRecordByHash(const char *szRecordCode, int offset, int len)
{
  if (!m_pdwHashList)
  {
    return GetRecord(szRecordCode, len + offset);
  }

  const unsigned int hash = MakeHash(&szRecordCode[offset], len);
  for (int n = 0; n < m_Header.m_nRecordNum; ++n)
  {
    if (m_pdwHashList[n] == hash)
    {
      return GetRecord(n);
    }
  }
  return nullptr;
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
  static unsigned char s_hashTable[256];
  static bool s_hashTableLoaded;

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

bool CRecordData::IsTableOpen() const
{
  return m_bLoad;
}

CRecordData::~CRecordData()
{
  if (m_ppsRecord != nullptr)
  {
    for (int j = 0; j < m_nLowNum; ++j)
    {
      if (m_ppsRecord[j] != nullptr)
      {
        delete[] m_ppsRecord[j];
        m_ppsRecord[j] = nullptr;
      }
    }
    delete[] m_ppsRecord;
    m_ppsRecord = nullptr;
  }

  if (m_pdwHashList != nullptr)
  {
    delete[] m_pdwHashList;
    m_pdwHashList = nullptr;
  }
}

