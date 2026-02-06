#include "pch.h"

#include "CUnmannedTraderRequestLimiter.h"

CUnmannedTraderRequestLimiter::CUnmannedTraderRequestLimiter()
  : m_eState(static_cast<REQUEST_TYPE>(-2))
{
}

void CUnmannedTraderRequestLimiter::ClearRequset()
{
  this->m_eState = static_cast<REQUEST_TYPE>(-1);
}
