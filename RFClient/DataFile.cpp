#include "DataFile.h"

#include <cstring>

CDataString::CDataString()
{
  ZeroMemory(this, sizeof(CDataString));
}

CDataString::~CDataString()
{
  if (m_pString)
  {
    delete[] m_pString;
    m_pString = NULL;
  }
}

DWORD CDataString::Read(void *pi_pBuffer, int pi_nSize, int pi_nReadCount)
{
  if (!pi_pBuffer)
  {
    return 0;
  }

  if (!pi_nSize || !pi_nReadCount)
  {
    return 0;
  }

  const DWORD l_dwHowMuchRead = pi_nSize * pi_nReadCount;
  if (l_dwHowMuchRead > (m_dwStringLength - m_dwCurPos))
  {
    return 0;
  }

  memcpy(pi_pBuffer, &m_pString[m_dwCurPos], l_dwHowMuchRead);
  m_dwCurPos += l_dwHowMuchRead;

  return l_dwHowMuchRead;
}

CDataFile::CDataFile()
{
  ZeroMemory(this, sizeof(CDataFile));
  m_hSourceFile = INVALID_HANDLE_VALUE;
}

CDataFile::CDataFile(char *pi_pFileName)
{
  ZeroMemory(this, sizeof(CDataFile));
  m_hSourceFile = INVALID_HANDLE_VALUE;

  LoadFile(pi_pFileName);
}

CDataFile::~CDataFile()
{
  if (m_hSourceFile != INVALID_HANDLE_VALUE)
  {
    CloseHandle(m_hSourceFile);
    m_hSourceFile = INVALID_HANDLE_VALUE;
  }
}

BOOL CDataFile::LoadFile(char *pi_pFileName)
{
  if (!pi_pFileName)
  {
    return FALSE;
  }

  m_hSourceFile = CreateFileA(pi_pFileName,
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              0);
  if (m_hSourceFile == INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }

  LoadFile_ReadFile();

  CloseHandle(m_hSourceFile);
  m_hSourceFile = INVALID_HANDLE_VALUE;

  DecodeEncryptionKey();
  DecodeSourceString();
  ZeroMemory(m_pKey, MAX_KEY_LENGTH);

  return TRUE;
}

BOOL CDataFile::LoadFile_ReadFile(void)
{
  LoadFile_ReadSection(m_pHeader, HEADER_LENGTH);

  DWORD l_dwSourceStringLength = 0;
  LoadFile_ReadSection(reinterpret_cast<char *>(&l_dwSourceStringLength), sizeof(DWORD));
  m_cSourceData.SetStringLength(l_dwSourceStringLength);

  m_cSourceData.SetString(new char[l_dwSourceStringLength + 1]);
  char *l_pSourceString = m_cSourceData.GetString();
  LoadFile_ReadSection(l_pSourceString, l_dwSourceStringLength);
  l_pSourceString[l_dwSourceStringLength] = '\0';

  LoadFile_ReadSection(m_pKey, MAX_KEY_LENGTH);
  return TRUE;
}

BOOL CDataFile::LoadFile_ReadSection(char *po_pBuffer, int pi_nSize)
{
  if (m_hSourceFile == INVALID_HANDLE_VALUE || !po_pBuffer)
  {
    return FALSE;
  }

  DWORD l_dwRealReadSize = 0;
  if (!ReadFile(m_hSourceFile, po_pBuffer, pi_nSize, &l_dwRealReadSize, NULL))
  {
    CloseHandle(m_hSourceFile);
    m_hSourceFile = INVALID_HANDLE_VALUE;
    return FALSE;
  }

  return TRUE;
}

BOOL CDataFile::DecodeSourceString(void)
{
  char *l_pSourceString = m_cSourceData.GetString();
  if (!l_pSourceString)
  {
    return FALSE;
  }

  BOOL l_bEven = TRUE;
  for (DWORD i = 0; i < m_cSourceData.GetStringLength(); ++i)
  {
    if (l_bEven)
    {
      l_pSourceString[i] -= m_pKey[(i + 1) % MAX_KEY_LENGTH];
      l_bEven = FALSE;
    }
    else
    {
      l_pSourceString[i] += m_pKey[(i + 1) % MAX_KEY_LENGTH];
      l_bEven = TRUE;
    }
  }

  return TRUE;
}

BOOL CDataFile::DecodeEncryptionKey(void)
{
  static const char DIGIT[8] = {1, 2, 4, 8, 16, 32, 64, static_cast<char>(128)};

  BOOL l_bEven = TRUE;
  for (int i = 0; i < MAX_KEY_LENGTH; ++i)
  {
    if (l_bEven)
    {
      m_pKey[i] -= DIGIT[(i + 1) % 8];
      l_bEven = FALSE;
    }
    else
    {
      m_pKey[i] += DIGIT[(i + 1) % 8];
      l_bEven = TRUE;
    }
  }

  for (int i = 0; i < MAX_KEY_LENGTH / 2; ++i)
  {
    const char l_nTemp = m_pKey[MAX_KEY_LENGTH - 1 - i];
    m_pKey[MAX_KEY_LENGTH - 1 - i] = m_pKey[i];
    m_pKey[i] = l_nTemp;
  }

  for (int i = 0; i < MAX_KEY_LENGTH; i += 2)
  {
    const char l_nTemp = m_pKey[i + 1];
    m_pKey[i + 1] = m_pKey[i];
    m_pKey[i] = l_nTemp;
  }

  return TRUE;
}
