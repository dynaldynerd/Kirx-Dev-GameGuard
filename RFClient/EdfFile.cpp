#include "EdfFile.h"

#include <cstring>

CEdfFile::CEdfFile()
  : m_pDataFile(NULL)
{
  ZeroMemory(m_szFileName, sizeof(m_szFileName));
}

CEdfFile::~CEdfFile()
{
  Unload();
}

BOOL CEdfFile::Load(const char *pi_pFileName)
{
  if (!pi_pFileName || !pi_pFileName[0])
  {
    return FALSE;
  }

  Unload();

  m_pDataFile = new CDataFile;
  if (!m_pDataFile)
  {
    return FALSE;
  }

  if (!m_pDataFile->LoadFile(const_cast<char *>(pi_pFileName)))
  {
    delete m_pDataFile;
    m_pDataFile = NULL;
    return FALSE;
  }

  strcpy_s(m_szFileName, sizeof(m_szFileName), pi_pFileName);
  return TRUE;
}

void CEdfFile::Unload(void)
{
  delete m_pDataFile;
  m_pDataFile = NULL;
  m_szFileName[0] = '\0';
}

BOOL CEdfFile::IsLoaded(void) const
{
  return m_pDataFile != NULL && m_pDataFile->GetSourceData() != NULL &&
         m_pDataFile->GetSourceData()->GetString() != NULL;
}

CDataString *CEdfFile::GetSourceData(void) const
{
  return m_pDataFile ? m_pDataFile->GetSourceData() : NULL;
}

const char *CEdfFile::GetFileName(void) const
{
  return m_szFileName;
}
