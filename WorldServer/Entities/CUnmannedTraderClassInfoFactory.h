#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderClassInfo.h"

#include <vector>

class  CUnmannedTraderClassInfoFactory
{
public:
  CUnmannedTraderClassInfoFactory();
  ~CUnmannedTraderClassInfoFactory();

  CUnmannedTraderClassInfo *Create(const char *szType, unsigned int dwID);
  bool Regist(CUnmannedTraderClassInfo *pkType);
  void Destroy();

  std::vector<CUnmannedTraderClassInfo *> m_vecTable;
};
