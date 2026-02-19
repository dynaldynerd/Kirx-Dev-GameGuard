#pragma once

#include <cstdarg>

#include "IdaCompat.h"
#include "CNetCriticalSection.h"

/* 1132 */
class CLogFile
{
public:
  CLogFile();
  ~CLogFile();
  void SetWriteLogFile(const char *szFileName, int bWriteAble, bool bTrace, bool bDate, bool bAddCount);
  void SetWriteAble(bool bAble);
  void Write(const char *format, ...);
  void WriteFromArg(const char *format, va_list arg);
  void WriteFromArg(const wchar_t *format, va_list arg);
  void WriteString(const char *fmt);

  char m_szFileName[128];
  unsigned int m_dwLogCount;
  int m_bWriteAble;
  CNetCriticalSection m_cs;
  bool m_bAddCount;
  bool m_bDate;
  bool m_bTrace;
  bool m_bInit;
};


