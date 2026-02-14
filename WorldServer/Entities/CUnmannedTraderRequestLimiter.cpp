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

bool CUnmannedTraderRequestLimiter::IsEmpty()
{
  return this->m_eState == static_cast<REQUEST_TYPE>(-1);
}

void CUnmannedTraderRequestLimiter::SetRequest(REQUEST_TYPE iRequest)
{
  if (static_cast<unsigned int>(iRequest) < 6u)
  {
    this->m_eState = iRequest;
  }
}
