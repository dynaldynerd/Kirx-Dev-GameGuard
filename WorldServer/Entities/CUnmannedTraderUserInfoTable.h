#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CUnmannedTraderUserInfo.h"
#include <vector>

class __cppobj CUnmannedTraderUserInfoTable
{
public:
  static CUnmannedTraderUserInfoTable *Instance();

  CUnmannedTraderUserInfoTable();
  bool Init();
  void SetLogger(CLogFile *pkLogger, CLogFile *pkServiceLogger);

  static CUnmannedTraderUserInfoTable *ms_Instance;

  CLogFile *m_pkLogger;
  CLogFile *m_pkServiceLogger;
  std::vector<CUnmannedTraderUserInfo> m_veckInfo;
};
