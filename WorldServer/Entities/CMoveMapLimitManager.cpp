#include "pch.h"

#include "CMoveMapLimitManager.h"
#include "CMoveMapLimitEnviromentValues.h"
#include "CPlayer.h"
#include "GlobalObjects.h"

CMoveMapLimitManager *CMoveMapLimitManager::ms_Instance = nullptr;

CMoveMapLimitManager::CMoveMapLimitManager()
{
}

CMoveMapLimitManager::~CMoveMapLimitManager()
{
}

CMoveMapLimitManager *CMoveMapLimitManager::Instance()
{
  if (!ms_Instance)
  {
    ms_Instance = new CMoveMapLimitManager();
  }
  return ms_Instance;
}

void CMoveMapLimitManager::Destroy()
{
  if (ms_Instance)
  {
    delete ms_Instance;
    ms_Instance = nullptr;
  }
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

void CMoveMapLimitManager::Load(CPlayer *pkPlayer)
{
  m_kRightInfo.Load(pkPlayer);
  CMoveMapLimitRightInfo *rightInfo = m_kRightInfo.Get(pkPlayer->m_ObjID.m_wIndex);
  m_kLimitInfo.Load(pkPlayer, rightInfo);
}

char CMoveMapLimitManager::MoveLimitMapZoneRequest(int iUserInx, char *pRequest)
{
  CPlayer *player = &g_Player[iUserInx];
  if (!player->m_bOper || player->m_pmTrd.bDTradeMode || player->m_bCorpse)
  {
    return 1;
  }

  Request(0, 2, player->m_pCurMap->m_nMapCode, *reinterpret_cast<unsigned int *>(pRequest), iUserInx, pRequest);
  return 1;
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
