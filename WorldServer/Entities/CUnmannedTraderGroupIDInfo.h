#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include <vector>

class CUnmannedTraderDivisionInfo;

class __cppobj CUnmannedTraderGroupIDInfo
{
public:
  CLogFile *m_pkLogger;
  std::vector<CUnmannedTraderDivisionInfo *> m_vecDivisionInfo;
};
