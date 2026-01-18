#pragma once

#include "IdaCompat.h"

/* 1132 */
class __cppobj __declspec(align(8)) CLogFile
{
  char m_szFileName[128];
  unsigned int m_dwLogCount;
  int m_bWriteAble;
  CNetCriticalSection m_cs;
  bool m_bAddCount;
  bool m_bDate;
  bool m_bTrace;
  bool m_bInit;
};

