#include "pch.h"

#include "CAsyncLogInfo.h"

#include "CLogFile.h"
#include "CMyTimer.h"
#include "WorldServerUtil.h"

#include <cstdio>
#include <cstring>

CAsyncLogInfo::CAsyncLogInfo()
  : m_eType(ALT_NONE),
    m_dwLogCount(0),
    m_szLogDirPath(nullptr),
    m_szLogFileName(nullptr),
    m_szTypeName(nullptr),
    m_pkTimer(nullptr)
{
}

CAsyncLogInfo::~CAsyncLogInfo()
{
  delete[] m_szLogDirPath;
  delete[] m_szLogFileName;
  delete[] m_szTypeName;
  delete m_pkTimer;
}

bool CAsyncLogInfo::Init(
  ASYNC_LOG_TYPE eType,
  const char *szDirPath,
  const char *szTypeName,
  bool bAddDateFileName,
  unsigned int dwUpdateFileNameDelay,
  CLogFile *logLoading)
{
  if (eType < ALT_ASYNC_LOGGER_SYSTEM_LOG || eType > ALT_GETEVENTCOUPON_LOG || szTypeName == nullptr)
  {
    if (logLoading != nullptr)
    {
      logLoading->Write(
        "CAsyncLogInfo::Init( int iType(%d), char * szDirPath(%s), char * szTypeName(%s) ) :  iType Invalid!",
        eType,
        szDirPath,
        szTypeName);
    }
    return false;
  }

  if (szDirPath == nullptr)
  {
    return false;
  }

  m_eType = eType;

  char buffer[10240]{};
  int len = 0;
  if (bAddDateFileName)
  {
    char szTime[256]{};
    if (!GetDateTimeStr(szTime))
    {
      return false;
    }
    len = sprintf_s(buffer, sizeof(buffer), "%s/%s_%s.log", szDirPath, szTypeName, szTime);
  }
  else
  {
    len = sprintf_s(buffer, sizeof(buffer), "%s/%s.log", szDirPath, szTypeName);
  }

  if (len <= 0)
  {
    if (logLoading != nullptr)
    {
      logLoading->Write(
        "CAsyncLogInfo::Init( int iType(%d), char * szDirPath(%s), char * szTypeName(%s) ) :  sprintf_s( %%s/%%s_%%d.log ) Ret(%d)!",
        eType,
        szDirPath,
        szTypeName,
        len);
    }
    return false;
  }

  m_szLogFileName = new char[static_cast<size_t>(len) + 1];
  if (m_szLogFileName == nullptr)
  {
    if (logLoading != nullptr)
    {
      logLoading->Write(
        "CAsyncLogInfo::Init( int iType(%d), char * szDirPath(%s), char * szTypeName(%s) ) :  m_szLogFileName = new char[iLen(%d)+1] NULL!",
        eType,
        szDirPath,
        szTypeName,
        len);
    }
    return false;
  }

  strcpy_s(m_szLogFileName, static_cast<size_t>(len) + 1, buffer);
  DeleteFileA(m_szLogFileName);

  len = sprintf_s(buffer, sizeof(buffer), "%s/%s", szDirPath, szTypeName);
  m_szLogDirPath = new char[static_cast<size_t>(len) + 1];
  if (m_szLogDirPath == nullptr)
  {
    if (logLoading != nullptr)
    {
      logLoading->Write(
        "CAsyncLogInfo::Init( int iType(%d), char * szDirPath(%s), szTypeName(%s) ) :  m_szLogDirPath = new char[iLen(%d)+1] NULL!",
        eType,
        szDirPath,
        szTypeName,
        len);
    }
    return false;
  }

  strcpy_s(m_szLogDirPath, static_cast<size_t>(len) + 1, buffer);

  len = static_cast<int>(std::strlen(szTypeName));
  m_szTypeName = new char[static_cast<size_t>(len) + 1];
  if (m_szTypeName == nullptr)
  {
    if (logLoading != nullptr)
    {
      logLoading->Write(
        "CAsyncLogInfo::Init( int iType(%d), char * szDirPath(%s), char * szTypeName(%s) ) :  m_szTypeName = new char[iLen(%d)+1] NULL!",
        eType,
        szDirPath,
        szTypeName,
        len);
    }
    return false;
  }

  strcpy_s(m_szTypeName, static_cast<size_t>(len) + 1, szTypeName);

  if (dwUpdateFileNameDelay == 0xFFFFFFFF)
  {
    return true;
  }

  if (dwUpdateFileNameDelay < 0x2710)
  {
    return false;
  }

  m_pkTimer = new CMyTimer();
  if (m_pkTimer == nullptr)
  {
    return false;
  }

  m_pkTimer->BeginTimer(dwUpdateFileNameDelay);
  return true;
}

char *CAsyncLogInfo::GetDirPath()
{
  return m_szLogDirPath;
}

char *CAsyncLogInfo::GetTypeName()
{
  return m_szTypeName;
}

char *CAsyncLogInfo::GetFileName()
{
  return m_szLogFileName;
}

unsigned int CAsyncLogInfo::GetCount()
{
  return m_dwLogCount;
}

void CAsyncLogInfo::IncreaseCount()
{
  ++m_dwLogCount;
}

void CAsyncLogInfo::UpdateLogFileName()
{
  if (m_pkTimer == nullptr)
  {
    return;
  }

  if (!m_pkTimer->CountingTimer())
  {
    return;
  }

  char szTime[256]{};
  if (!GetDateTimeStr(szTime))
  {
    return;
  }

  char buffer[10240]{};
  const size_t sizeInBytes = std::strlen(m_szLogFileName) + 1;
  const int len = sprintf_s(buffer, sizeof(buffer), "%s_%s.log", m_szLogDirPath, szTime);
  if (len <= 0)
  {
    return;
  }

  m_csLock.Lock();
  strcpy_s(m_szLogFileName, sizeInBytes, buffer);
  m_csLock.Unlock();
}

