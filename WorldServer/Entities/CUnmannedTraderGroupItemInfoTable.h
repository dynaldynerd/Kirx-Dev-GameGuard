#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CUnmannedTraderGroupIDInfo.h"
#include "CUnmannedTraderGroupVersionInfo.h"

class __cppobj CUnmannedTraderGroupItemInfoTable
{
public:
  CLogFile *m_pkLogger;
  CUnmannedTraderGroupVersionInfo m_kVerInfo;
  CUnmannedTraderGroupIDInfo m_kGroupIDInfo;
};
