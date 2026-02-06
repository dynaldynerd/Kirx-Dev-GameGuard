#pragma once

#include "IdaCompat.h"

class __cppobj ControllerTaxRate
{
public:
  void setLimitTaxRate(float fMin, float fMax);
  bool checkLimitTaxRate(float fTaxRate);

  bool m_bInit;
  float m_fMinTaxRate;
  float m_fMaxTaxRate;
  float m_fCurTaxRate;
};
