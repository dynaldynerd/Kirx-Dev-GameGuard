#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderItemCodeInfo.h"
#include "CUnmannedTraderSubClassInfo.h"
#include <vector>

class __cppobj CUnmannedTraderSubClassInfoCode : public CUnmannedTraderSubClassInfo
{
public:
  std::vector<CUnmannedTraderItemCodeInfo> m_vecCodeList;
};
