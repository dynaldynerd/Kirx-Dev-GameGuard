#include "pch.h"

#include "strFILE.h"

#include <cstdlib>
#include <windows.h>

strFILE::strFILE()
  : m_pLoadStr(nullptr),
    m_pReadStr(nullptr),
    m_dwLoadSize(0)
{
}

strFILE::~strFILE()
{

  if (m_pLoadStr)
    operator delete[](m_pLoadStr);
}

bool strFILE::load(const char *pszFileName)
{

  if (!pszFileName)
    return false;
  if (m_pLoadStr)
    return false;

  HANDLE hFile = CreateFileA(pszFileName, 0x80000000, 1u, nullptr, 3u, 0x80u, nullptr);
  if (hFile == reinterpret_cast<HANDLE>(-1))
    return false;

  const DWORD start = SetFilePointer(hFile, 0, nullptr, 0);
  const DWORD end = SetFilePointer(hFile, 0, nullptr, 2u);
  m_dwLoadSize = end - start;
  if (!m_dwLoadSize)
  {
    CloseHandle(hFile);
    return false;
  }

  auto *buffer = static_cast<char *>(operator new[](m_dwLoadSize + 1));
  m_pLoadStr = buffer;
  SetFilePointer(hFile, 0, nullptr, 0);

  unsigned int bytesRead = 0;
  ReadFile(hFile, m_pLoadStr, m_dwLoadSize, reinterpret_cast<DWORD *>(&bytesRead), nullptr);
  m_pLoadStr[m_dwLoadSize] = '\0';
  CloseHandle(hFile);

  m_pReadStr = m_pLoadStr;
  return true;
}

bool strFILE::word(char *poutszWord)
{

  if (!m_pReadStr)
    return false;
  if (!*m_pReadStr)
    return false;

  while (m_pReadStr
         && *m_pReadStr
         && (*m_pReadStr == 32 || *m_pReadStr == 10 || *m_pReadStr == 9 || *m_pReadStr == 13))
    ++m_pReadStr;

  if (!m_pReadStr)
    return false;

  int wordLen = 0;
  void *src = m_pReadStr;
  while (m_pReadStr
         && *m_pReadStr
         && *m_pReadStr != 32
         && *m_pReadStr != 10
         && *m_pReadStr != 9
         && *m_pReadStr != 13)
  {
    ++m_pReadStr;
    ++wordLen;
  }

  if (wordLen <= 0)
    return false;

  std::memcpy(poutszWord, src, wordLen);
  poutszWord[wordLen] = '\0';
  return true;
}

bool strFILE::word(int *pnoutVal)
{

  char buffer[144]{};
  if (!word(buffer))
    return false;
  *pnoutVal = atoi(buffer);
  return true;
}

bool strFILE::word(float *pfoutVal)
{

  char buffer[144]{};
  if (!word(buffer))
    return false;
  *pfoutVal = static_cast<float>(atof(buffer));
  return true;
}

bool strFILE::word(long double *pdoutVal)
{

  char buffer[144]{};
  if (!word(buffer))
    return false;
  *pdoutVal = atof(buffer);
  return true;
}

int strFILE::read_line_count()
{
  int lineCount = 0;
  char *readCursor = m_pLoadStr;
  for (unsigned int index = 0; index < m_dwLoadSize && readCursor != m_pReadStr; ++index)
  {
    if (*readCursor == 10)
      ++lineCount;
    ++readCursor;
  }
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<unsigned int>(lineCount + 1));
}
