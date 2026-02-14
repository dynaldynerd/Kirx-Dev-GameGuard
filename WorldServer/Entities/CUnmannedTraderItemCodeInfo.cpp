#include "pch.h"

#include "CUnmannedTraderItemCodeInfo.h"
#include <cstdlib>
#include <cstring>

CUnmannedTraderItemCodeInfo::CUnmannedTraderItemCodeInfo(
  const char *szCode,
  unsigned int dwStartIndex,
  unsigned int dwEndIndex)
  : m_dwStartInx(dwStartIndex), m_dwEndInx(dwEndIndex), m_szCode{}
{
  this->m_szCode[0] = '\0';
  if (szCode)
  {
    strcpy_0(this->m_szCode, szCode);
    this->m_szCode[63] = '\0';
  }
}

CUnmannedTraderItemCodeInfo::CUnmannedTraderItemCodeInfo(const CUnmannedTraderItemCodeInfo &lhs)
  : m_dwStartInx(0), m_dwEndInx(0), m_szCode{}
{
  operator=(lhs);
}

CUnmannedTraderItemCodeInfo::~CUnmannedTraderItemCodeInfo()
{
  m_dwStartInx = 0;
  m_dwEndInx = 0;
  m_szCode[0] = '\0';
}

const CUnmannedTraderItemCodeInfo *CUnmannedTraderItemCodeInfo::operator=(const CUnmannedTraderItemCodeInfo &lhs)
{
  this->m_dwStartInx = lhs.m_dwStartInx;
  this->m_dwEndInx = lhs.m_dwEndInx;
  this->m_szCode[0] = '\0';
  strcpy_0(this->m_szCode, lhs.m_szCode);
  this->m_szCode[63] = '\0';
  return this;
}

bool CUnmannedTraderItemCodeInfo::operator==(const char *szCode)
{
  if (!szCode)
  {
    return false;
  }

  const size_t len = strlen_0(this->m_szCode);
  if (strncmp(this->m_szCode, szCode, len))
  {
    return false;
  }

  if (!this->m_dwStartInx && !this->m_dwEndInx)
  {
    return true;
  }

  const char *digits = strpbrk(szCode, "0123456789");
  if (!digits)
  {
    return false;
  }

  const unsigned int value = static_cast<unsigned int>(atoi(digits));
  return this->m_dwStartInx <= value && value <= this->m_dwEndInx;
}
