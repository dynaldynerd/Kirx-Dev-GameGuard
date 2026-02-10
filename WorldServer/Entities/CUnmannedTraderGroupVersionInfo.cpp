#include "pch.h"

#include "CUnmannedTraderGroupVersionInfo.h"
#include "CUnmannedTraderGroupDivisionVersionInfo.h"
#include <utility>

CUnmannedTraderGroupVersionInfo::CUnmannedTraderGroupVersionInfo()
  : m_vecVerInfo()
{
}

bool CUnmannedTraderGroupVersionInfo::Init(std::vector<std::pair<unsigned long, unsigned long>> *vecInfo)
{
  if (vecInfo->empty())
  {
    return false;
  }

  for (const std::pair<unsigned long, unsigned long> &entry : *vecInfo)
  {
    CUnmannedTraderGroupDivisionVersionInfo info(static_cast<int>(entry.first), entry.second);
    this->m_vecVerInfo.push_back(info);

    CUnmannedTraderGroupDivisionVersionInfo &back = this->m_vecVerInfo.back();
    if (back.IsEmpty())
    {
      return false;
    }
  }

  return this->m_vecVerInfo.size() == vecInfo->size();
}

bool CUnmannedTraderGroupVersionInfo::IncreaseVersion(unsigned __int8 byDivision, unsigned __int8 byClass)
{
  if (this->m_vecVerInfo.size() <= byDivision)
  {
    return false;
  }

  return this->m_vecVerInfo[byDivision].IncreaseVersion(byClass) != 0;
}
