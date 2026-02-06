#include "pch.h"

#include "ControllerTaxRate.h"

void ControllerTaxRate::setLimitTaxRate(float fMin, float fMax)
{
  this->m_fMinTaxRate = fMin;
  this->m_fMaxTaxRate = fMax;
}

bool ControllerTaxRate::checkLimitTaxRate(float fTaxRate)
{
  return fTaxRate >= this->m_fMinTaxRate && this->m_fMaxTaxRate >= fTaxRate;
}
