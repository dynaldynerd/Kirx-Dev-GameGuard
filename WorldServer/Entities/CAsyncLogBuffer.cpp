#include "pch.h"

#include "CAsyncLogBuffer.h"

#include <cstdio>
#include <cstring>

CAsyncLogBuffer::CAsyncLogBuffer()
  : m_eState(ALBS_NONE),
    m_iLen(0),
    m_iMaxBuffSize(0),
    m_szBuffer(nullptr)
{
  m_szFileName[0] = '\0';
}

CAsyncLogBuffer::~CAsyncLogBuffer()
{
  delete[] m_szBuffer;
}

bool CAsyncLogBuffer::Init(int iMaxBufferSize)
{
  if (iMaxBufferSize == 0)
  {
    return false;
  }

  m_szBuffer = new char[iMaxBufferSize];
  if (m_szBuffer == nullptr)
  {
    return false;
  }

  m_iMaxBuffSize = iMaxBufferSize;
  return true;
}

bool CAsyncLogBuffer::Log(const char *pszFileName, const char *szLog, int iLen)
{
  if (m_iMaxBuffSize == 0 || (m_iMaxBuffSize - 3) < iLen)
  {
    return false;
  }

  if (m_szBuffer == nullptr)
  {
    return false;
  }

  m_szBuffer[0] = '\0';
  strcpy_s(m_szFileName, sizeof(m_szFileName), pszFileName);
  m_iLen = sprintf_s(m_szBuffer, m_iMaxBuffSize, "%s\r\n", szLog);
  return m_iLen >= 0;
}

char *CAsyncLogBuffer::GetFileName()
{
  return m_szFileName;
}

int CAsyncLogBuffer::GetLength()
{
  return m_iLen;
}

char *CAsyncLogBuffer::GetStr()
{
  return m_szBuffer;
}

