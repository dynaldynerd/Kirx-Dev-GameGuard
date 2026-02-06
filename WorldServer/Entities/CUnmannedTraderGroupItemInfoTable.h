#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CUnmannedTraderGroupIDInfo.h"
#include "CUnmannedTraderGroupVersionInfo.h"

class __cppobj CUnmannedTraderGroupItemInfoTable
{
public:
  static CUnmannedTraderGroupItemInfoTable *Instance();

  CUnmannedTraderGroupItemInfoTable();
  bool Init();
  void SetLogger(CLogFile *pkLogger);
  void Log(const char *fmt, ...);

  static CUnmannedTraderGroupItemInfoTable *ms_Instance;

  CLogFile *m_pkLogger;
  CUnmannedTraderGroupVersionInfo m_kVerInfo;
  CUnmannedTraderGroupIDInfo m_kGroupIDInfo;
};
