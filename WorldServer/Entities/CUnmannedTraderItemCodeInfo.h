#pragma once

#include "IdaCompat.h"

class __cppobj CUnmannedTraderItemCodeInfo
{
public:
  CUnmannedTraderItemCodeInfo(const char *szCode, unsigned int dwStartIndex, unsigned int dwEndIndex);
  CUnmannedTraderItemCodeInfo(const CUnmannedTraderItemCodeInfo &lhs);
  ~CUnmannedTraderItemCodeInfo();
  const CUnmannedTraderItemCodeInfo *operator=(const CUnmannedTraderItemCodeInfo &lhs);
  bool operator==(const char *szCode);

  unsigned int m_dwStartInx;
  unsigned int m_dwEndInx;
  char m_szCode[64];
};
