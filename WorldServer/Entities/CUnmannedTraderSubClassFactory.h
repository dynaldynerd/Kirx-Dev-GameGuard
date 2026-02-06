#pragma once

#include "IdaCompat.h"
#include <vector>

class CUnmannedTraderSubClassInfo;

class __cppobj CUnmannedTraderSubClassFactory
{
public:
  CUnmannedTraderSubClassFactory();
  ~CUnmannedTraderSubClassFactory();

  CUnmannedTraderSubClassInfo *Create(const char *szType, unsigned int dwID);
  bool Regist(CUnmannedTraderSubClassInfo *pkType);
  void Destroy();

  std::vector<CUnmannedTraderSubClassInfo *> m_vecTable;
};
