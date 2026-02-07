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

float ControllerTaxRate::getCurTaxRate()
{
  return m_fCurTaxRate;
}

void ControllerTaxRate::setCurTaxRate(float fTaxRate)
{
  m_fCurTaxRate = fTaxRate;
}

unsigned int ControllerTaxRate::calcTaxRate(unsigned int dalant)
{
  return static_cast<unsigned int>(static_cast<int>(static_cast<float>(dalant) * m_fCurTaxRate));
}
