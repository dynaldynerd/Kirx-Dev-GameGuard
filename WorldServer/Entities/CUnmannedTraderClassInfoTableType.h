#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderClassInfo.h"

#include <vector>

class CUnmannedTraderSubClassInfo;

class __cppobj CUnmannedTraderClassInfoTableType : public CUnmannedTraderClassInfo
{
public:
  unsigned char m_byTableCode;
  std::vector<CUnmannedTraderSubClassInfo *> m_vecSubClass;
};
