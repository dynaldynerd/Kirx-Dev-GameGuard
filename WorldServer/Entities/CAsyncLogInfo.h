#pragma once

#include "IdaCompat.h"
#include "AsyncLogTypes.h"
#include "CNetCriticalSection.h"

class CLogFile;
class CMyTimer;

class __cppobj CAsyncLogInfo
{
public:
  CAsyncLogInfo();
  ~CAsyncLogInfo();

  bool Init(
    ASYNC_LOG_TYPE eType,
    const char *szDirPath,
    const char *szTypeName,
    bool bAddDateFileName,
    unsigned int dwUpdateFileNameDelay,
    CLogFile *logLoading);

  char *GetDirPath();
  char *GetTypeName();
  char *GetFileName();
  unsigned int GetCount();
  void IncreaseCount();
  void UpdateLogFileName();

  ASYNC_LOG_TYPE m_eType;
  unsigned int m_dwLogCount;
  char *m_szLogDirPath;
  char *m_szLogFileName;
  char *m_szTypeName;
  CMyTimer *m_pkTimer;
  CNetCriticalSection m_csLock;
};

