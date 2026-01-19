#pragma once

#include "IdaCompat.h"
#include "CMyTimer.h"
#include "TRC_AutoTrade.h"
#include <vector>

class __cppobj CUnmannedTraderTaxRateManager
{
public:
  CMyTimer *m_pkTimer;
  std::vector<TRC_AutoTrade *> m_vecTRC;
};
