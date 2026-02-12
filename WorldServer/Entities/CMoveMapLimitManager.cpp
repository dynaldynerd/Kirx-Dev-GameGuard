#include "pch.h"

#include "CMoveMapLimitManager.h"
#include "CMoveMapLimitEnviromentValues.h"
#include "CPlayer.h"

CMoveMapLimitManager *CMoveMapLimitManager::Instance()
{
  static CMoveMapLimitManager s_instance;
  return &s_instance;
}

void CMoveMapLimitManager::Loop()
{
  m_kLimitInfo.Loop();
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

void CMoveMapLimitManager::LogIn(CPlayer *pkPlayer)
{
  m_kRightInfo.LogIn(pkPlayer);
  CMoveMapLimitRightInfo *rightInfo = m_kRightInfo.Get(pkPlayer->m_ObjID.m_wIndex);
  m_kLimitInfo.LogIn(pkPlayer, rightInfo);
}

void CMoveMapLimitManager::LogOut(CPlayer *pkPlayer)
{
  m_kRightInfo.LogOut(pkPlayer);
  CMoveMapLimitRightInfo *rightInfo = m_kRightInfo.Get(pkPlayer->m_ObjID.m_wIndex);
  m_kLimitInfo.LogOut(pkPlayer, rightInfo);
}

void CMoveMapLimitManager::CreateComplete(CPlayer *pkPlayer)
{
  m_kRightInfo.CreateComplete(pkPlayer);
}
