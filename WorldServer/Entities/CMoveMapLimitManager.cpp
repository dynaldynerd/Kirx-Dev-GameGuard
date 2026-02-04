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

bool CMoveMapLimitManager::Request(
  int iLimitType,
  int iRequetType,
  int iMapInx,
  unsigned int dwStoreRecordIndex,
  int iUserInx,
  char *pRequest)
{
  CMoveMapLimitRightInfo *rightInfo = m_kRightInfo.Get(iUserInx);
  return m_kLimitInfo.Request(iLimitType, iRequetType, iMapInx, dwStoreRecordIndex, iUserInx, pRequest, rightInfo);
}

bool CMoveMapLimitManager::RequestElanMapUserForceMoveHQ()
{
  return Request(
    0,
    0,
    CMoveMapLimitEnviromentValues::ELAN_MAP_CODE,
    CMoveMapLimitEnviromentValues::ELAN_1TH_LIMIT_NPC_RECORD_INDEX,
    -1,
    nullptr);
}
