#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CUnmannedTraderUserInfo.h"
#include <vector>

class __cppobj CUnmannedTraderUserInfoTable
{
public:
  CLogFile *m_pkLogger;
  CLogFile *m_pkServiceLogger;
  std::vector<CUnmannedTraderUserInfo> m_veckInfo;
};
