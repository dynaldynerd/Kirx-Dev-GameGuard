#pragma once

#include "IdaCompat.h"
#include <vector>

class CUnmannedTraderClassInfo;
class CUnmannedTraderSortType;

class __cppobj CUnmannedTraderDivisionInfo
{
public:
  unsigned int m_dwID;
  char m_szName[128];
  std::vector<CUnmannedTraderClassInfo *> m_vecClass;
  std::vector<CUnmannedTraderSortType *> m_vecSortType;
};
