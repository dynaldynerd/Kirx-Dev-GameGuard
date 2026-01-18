#pragma once

#include "IdaCompat.h"

/* 1126 */
struct __cppobj __declspec(align(8)) CRFNewDatabase
{
  void *m_hEnv;
  void *m_hDbc;
  void *m_hStmtSelect;
  void *m_hStmtUpdate;
  bool m_bConectionActive;
  bool m_bSaveDBLog;
  CLogFile m_ProcessLogW;
  CLogFile m_ErrorLogW;
  CLogFile m_ProcessLogA;
  CLogFile m_ErrorLogA;
  unsigned __int8 m_byLogFileHour;
  char m_szOdbcName[32];
  char m_szAccountName[32];
  char m_szPassword[32];
  bool m_bReconnectFailExit;
  char m_szLogUpperPath[128];
  virtual ~CRFNewDatabase() = default;
};
