#pragma once

#include "IdaCompat.h"
#include "CAsyncLogBuffer.h"
#include "CNetIndexList.h"

class CLogFile;

class __cppobj CAsyncLogBufferList
{
public:
  CAsyncLogBufferList();
  ~CAsyncLogBufferList();

  bool Init(unsigned int uiMaxBufferCnt, unsigned int uiMaxBufferSize, CLogFile *logLoading);
  bool Log(const char *pszFileName, const char *szLog, int iLen);
  void ProcWrite();
  static void WriteFile(const char *pszFileName, unsigned long nLen, const char *pszData);

  int GetBufferSize();
  int GetEmptySize();
  int GetProcCount();

  unsigned int m_uiMaxBufferCnt;
  unsigned int m_uiMaxBufferSize;
  CAsyncLogBuffer *m_pLogBuffer;
  CNetIndexList m_klistEmpty;
  CNetIndexList m_klistProc;
};

