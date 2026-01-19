#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderClassInfoTableType.h"
#include "CUnmannedTraderItemCodeInfo.h"

#include <vector>

class __cppobj CUnmannedTraderClassInfoTableCodeType : public CUnmannedTraderClassInfoTableType
{
public:
  std::vector<CUnmannedTraderItemCodeInfo> m_vecCodeList;
};
