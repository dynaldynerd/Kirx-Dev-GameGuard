#include "pch.h"

#include "CUnmannedTraderGroupDivisionVersionInfo.h"

CUnmannedTraderGroupDivisionVersionInfo::CUnmannedTraderGroupDivisionVersionInfo(
  int iType,
  unsigned int uiMaxCnt)
  : m_iType(iType), m_vecuiVersion(uiMaxCnt, 1)
{
}

bool CUnmannedTraderGroupDivisionVersionInfo::IsEmpty()
{
  return this->m_iType < 0 || this->m_vecuiVersion.empty();
}
