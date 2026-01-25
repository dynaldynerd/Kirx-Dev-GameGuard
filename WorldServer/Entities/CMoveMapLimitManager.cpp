#include "pch.h"

#include "CMoveMapLimitManager.h"
#include "CMoveMapLimitEnviromentValues.h"

CMoveMapLimitManager *CMoveMapLimitManager::Instance()
{
  static CMoveMapLimitManager s_instance;
  return &s_instance;
}

bool CMoveMapLimitManager::Init()
{
  if (!CMoveMapLimitEnviromentValues::Init())
  {
    return false;
  }

  std::vector<int> vecRightTypeList;
  if (!m_kLimitInfo.Init(&vecRightTypeList))
  {
    return false;
  }
  if (!m_kRightInfo.Init(&vecRightTypeList))
  {
    return false;
  }
  return true;
}
