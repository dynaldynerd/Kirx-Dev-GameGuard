#include "pch.h"

#include "CAsyncLogBufferList.h"

#include "CLogFile.h"

CAsyncLogBufferList::CAsyncLogBufferList()
  : m_uiMaxBufferCnt(0),
    m_uiMaxBufferSize(0),
    m_pLogBuffer(nullptr)
{
}

CAsyncLogBufferList::~CAsyncLogBufferList()
{
  delete[] m_pLogBuffer;
  m_pLogBuffer = nullptr;
}

bool CAsyncLogBufferList::Init(unsigned int uiMaxBufferCnt, unsigned int uiMaxBufferSize, CLogFile *logLoading)
{
  if (uiMaxBufferCnt == 0 || uiMaxBufferSize < 4)
  {
    return false;
  }

  m_pLogBuffer = new CAsyncLogBuffer[uiMaxBufferCnt];
  if (m_pLogBuffer == nullptr)
  {
    if (logLoading != nullptr)
    {
      logLoading->Write(
        "CAsyncLogBufferList::Init( UINT uiMaxBufferCnt(%u), UINT uiMaxBufferSize(%u) ) : new CAsyncLogBuffer[uiMaxBufferCnt] NULL!",
        uiMaxBufferCnt,
        uiMaxBufferSize);
    }
    return false;
  }

  for (unsigned int j = 0; j < uiMaxBufferCnt; ++j)
  {
    if (!m_pLogBuffer[j].Init(static_cast<int>(uiMaxBufferSize)))
    {
      if (logLoading != nullptr)
      {
        logLoading->Write(
          "CAsyncLogBufferList::Init( UINT uiMaxBufferCnt(%u), UINT uiMaxBufferSize(%u) ) : m_pLogBuffer[i(%u)]->Init( uiMaxBufferSize ) Fail!",
          uiMaxBufferCnt,
          uiMaxBufferSize,
          j);
      }
      return false;
    }
  }

  m_uiMaxBufferCnt = uiMaxBufferCnt;
  m_uiMaxBufferSize = uiMaxBufferSize;

  m_klistEmpty.SetList(uiMaxBufferCnt);
  for (unsigned int idx = 0; idx < uiMaxBufferCnt; ++idx)
  {
    m_klistEmpty.PushNode_Back(idx);
  }
  m_klistProc.SetList(uiMaxBufferCnt);
  return true;
}

bool CAsyncLogBufferList::Log(const char *pszFileName, const char *szLog, int iLen)
{
  if (m_pLogBuffer == nullptr)
  {
    return false;
  }

  unsigned int outIndex = 0;
  if (!m_klistEmpty.PopNode_Front(&outIndex))
  {
    return false;
  }

  if (!m_pLogBuffer[outIndex].Log(pszFileName, szLog, iLen))
  {
    return false;
  }

  m_klistProc.PushNode_Back(outIndex);
  return true;
}

void CAsyncLogBufferList::ProcWrite()
{
  unsigned int outIndex = 0;
  while (m_klistProc.PopNode_Front(&outIndex))
  {
    char *pszData = m_pLogBuffer[outIndex].GetStr();
    int nLen = m_pLogBuffer[outIndex].GetLength();
    const char *pszFileName = m_pLogBuffer[outIndex].GetFileName();
    WriteFile(pszFileName, static_cast<unsigned long>(nLen), pszData);
    m_klistEmpty.PushNode_Back(outIndex);
    Sleep(0);
  }
}

void CAsyncLogBufferList::WriteFile(const char *pszFileName, unsigned long nLen, const char *pszData)
{
  HANDLE hFile = CreateFileA(pszFileName, GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    return;
  }

  SetFilePointer(hFile, 0, nullptr, FILE_END);
  DWORD bytesWritten = 0;
  ::WriteFile(hFile, pszData, nLen, &bytesWritten, nullptr);
  CloseHandle(hFile);
}

int CAsyncLogBufferList::GetBufferSize()
{
  return static_cast<int>(m_uiMaxBufferSize) - 3;
}

int CAsyncLogBufferList::GetEmptySize()
{
  return static_cast<int>(m_klistEmpty.size());
}

int CAsyncLogBufferList::GetProcCount()
{
  return static_cast<int>(m_klistProc.size());
}

