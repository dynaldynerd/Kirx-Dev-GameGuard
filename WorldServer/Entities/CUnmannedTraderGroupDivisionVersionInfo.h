#pragma once

#include "IdaCompat.h"
#include <vector>

class  CUnmannedTraderGroupDivisionVersionInfo
{
public:
  CUnmannedTraderGroupDivisionVersionInfo(int iType, unsigned int uiMaxCnt);
  ~CUnmannedTraderGroupDivisionVersionInfo();
  bool IsEmpty();
  bool GetVersion(unsigned __int8 byClass, unsigned int *dwVer);
  bool IncreaseVersion(unsigned __int8 byClass);

  int m_iType;
  std::vector<unsigned long> m_vecuiVersion;
};
