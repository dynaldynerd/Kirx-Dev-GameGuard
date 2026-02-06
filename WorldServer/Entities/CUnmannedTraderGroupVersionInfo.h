#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderGroupDivisionVersionInfo.h"
#include <vector>

class __cppobj CUnmannedTraderGroupVersionInfo
{
public:
  CUnmannedTraderGroupVersionInfo();
  bool Init(std::vector<std::pair<unsigned long, unsigned long>> *vecInfo);

  std::vector<CUnmannedTraderGroupDivisionVersionInfo> m_vecVerInfo;
};
