#pragma once

#include "IdaCompat.h"

class __cppobj ControllerTaxRate
{
public:
  ControllerTaxRate();
  virtual ~ControllerTaxRate();

  void setLimitTaxRate(float fMin, float fMax);
  bool checkLimitTaxRate(float fTaxRate);
  float getCurTaxRate();
  void setCurTaxRate(float fTaxRate);
  unsigned int calcTaxRate(unsigned int dalant);

  bool m_bInit;
  float m_fMinTaxRate;
  float m_fMaxTaxRate;
  float m_fCurTaxRate;
};
