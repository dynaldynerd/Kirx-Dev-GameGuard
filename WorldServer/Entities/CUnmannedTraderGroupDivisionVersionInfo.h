#pragma once

#include "IdaCompat.h"
#include <vector>

class __cppobj CUnmannedTraderGroupDivisionVersionInfo
{
public:
  CUnmannedTraderGroupDivisionVersionInfo(int iType, unsigned int uiMaxCnt);
  ~CUnmannedTraderGroupDivisionVersionInfo();
  bool IsEmpty();
  char GetVersion(unsigned __int8 byClass, unsigned int *dwVer);
  char IncreaseVersion(unsigned __int8 byClass);

  int m_iType;
  std::vector<unsigned long> m_vecuiVersion;
};
