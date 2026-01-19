#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CUnmannedTraderLazyCleaner.h"
#include "CUnmannedTraderTradeInfo.h"

class __cppobj CUnmannedTraderController
{
public:
  CLogFile *m_pkLogger;
  CLogFile *m_pkServiceLogger;
  CUnmannedTraderTradeInfo m_kTradeInfo;
  CUnmannedTraderLazyCleaner m_kLazyCleaner;
};
