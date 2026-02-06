#include "pch.h"

#include "CUnmannedTraderLazyCleaner.h"
#include "CUnmannedTraderEnvironmentValue.h"

bool CUnmannedTraderLazyCleaner::Init()
{
  this->m_pkTimer = new CMyTimer();
  if (!this->m_pkTimer)
  {
    return false;
  }

  this->m_pkTimer->BeginTimer(CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADER_LAZYCLEANER_LOOP_DELAY);
  return true;
}
