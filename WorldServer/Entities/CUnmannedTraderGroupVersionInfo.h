#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderGroupDivisionVersionInfo.h"
#include <vector>

class __cppobj CUnmannedTraderGroupVersionInfo
{
public:
  CUnmannedTraderGroupVersionInfo();
  bool Init(std::vector<std::pair<unsigned long, unsigned long>> *vecInfo);
  bool GetVersion(unsigned __int8 byDivision, unsigned __int8 byClass, unsigned int *dwVer);
  bool IncreaseVersion(unsigned __int8 byDivision, unsigned __int8 byClass);

  std::vector<CUnmannedTraderGroupDivisionVersionInfo> m_vecVerInfo;
};
