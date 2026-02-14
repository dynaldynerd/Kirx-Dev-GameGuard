#include "pch.h"

#include "CUnmannedTraderGroupDivisionVersionInfo.h"

CUnmannedTraderGroupDivisionVersionInfo::CUnmannedTraderGroupDivisionVersionInfo(
  int iType,
  unsigned int uiMaxCnt)
  : m_iType(iType), m_vecuiVersion(uiMaxCnt, 1)
{
}

CUnmannedTraderGroupDivisionVersionInfo::~CUnmannedTraderGroupDivisionVersionInfo()
{
  m_vecuiVersion.clear();
}

bool CUnmannedTraderGroupDivisionVersionInfo::IsEmpty()
{
  return this->m_iType < 0 || this->m_vecuiVersion.empty();
}

char CUnmannedTraderGroupDivisionVersionInfo::GetVersion(unsigned __int8 byClass, unsigned int *dwVer)
{
  if (this->m_vecuiVersion.empty() || this->m_vecuiVersion.size() <= byClass)
  {
    return 0;
  }

  *dwVer = this->m_vecuiVersion[byClass];
  return 1;
}

char CUnmannedTraderGroupDivisionVersionInfo::IncreaseVersion(unsigned __int8 byClass)
{
  if (this->m_vecuiVersion.empty() || this->m_vecuiVersion.size() <= byClass)
  {
    return 0;
  }

  ++this->m_vecuiVersion[byClass];
  return 1;
}
