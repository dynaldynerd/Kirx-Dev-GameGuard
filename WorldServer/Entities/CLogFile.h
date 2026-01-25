#pragma once

#include "IdaCompat.h"
#include "CNetCriticalSection.h"

/* 1132 */
class __cppobj __declspec(align(8)) CLogFile
{
public:
  void SetWriteLogFile(const char *fileName, int addCount, int date, int trace, int init);
  void Write(const char *format, ...);

  char m_szFileName[128];
  unsigned int m_dwLogCount;
  int m_bWriteAble;
  CNetCriticalSection m_cs;
  bool m_bAddCount;
  bool m_bDate;
  bool m_bTrace;
  bool m_bInit;
};

