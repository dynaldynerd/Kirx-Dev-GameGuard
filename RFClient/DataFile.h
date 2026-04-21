#pragma once

#include <windows.h>

#define MAX_KEY_LENGTH (256)
#define HEADER_LENGTH  (29)

class CDataString
{
  DWORD m_dwStringLength;
  DWORD m_dwCurPos;
  char *m_pString;

public:
  CDataString();
  ~CDataString();

  DWORD Read(void *pi_pBuffer, int pi_nSize, int pi_nReadCount);

  void SetStringLength(DWORD pi_dwLength) { m_dwStringLength = pi_dwLength; }
  DWORD GetStringLength(void) { return m_dwStringLength; }
  void SetString(char *pi_pString) { m_pString = pi_pString; }
  char *GetString(void) { return m_pString; }
};

class CDataFile
{
protected:
  char m_pHeader[HEADER_LENGTH];
  char m_pKey[MAX_KEY_LENGTH];
  CDataString m_cSourceData;

  HANDLE m_hSourceFile;

public:
  CDataFile();
  CDataFile(char *pi_pFileName);
  ~CDataFile();

  BOOL LoadFile(char *pi_pFileName);

  CDataString *GetSourceData(void) { return &m_cSourceData; }

protected:
  BOOL LoadFile_ReadFile(void);
  BOOL LoadFile_ReadSection(char *po_pBuffer, int pi_nSize);

  BOOL DecodeSourceString(void);
  BOOL DecodeEncryptionKey(void);
};
