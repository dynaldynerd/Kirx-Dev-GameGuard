#include "pch.h"

#include "CMoveMapLimitRightInfoList.h"

#include <algorithm>

#include "GlobalObjects.h"

bool CMoveMapLimitRightInfoList::Init(const std::vector<int> *vecRightTypeList)
{
  CMoveMapLimitRightInfo baseInfo;
  for (int type : *vecRightTypeList)
  {
    if (!baseInfo.Regist(type))
    {
      g_Main.m_logLoadingError.Write(
        "CMoveMapLimitRightInfoList::Init(...) : kVal.Regist( iType(%d) ) ) Fail!",
        type);
      return false;
    }
  }

  CMoveMapLimitRightInfo defaultInfo;
  m_vecRight.assign(MAX_PLAYER, defaultInfo);
  if (m_vecRight.size() != MAX_PLAYER)
  {
    g_Main.m_logLoadingError.Write(
      "CMoveMapLimitRightInfoList::Init(...) : m_vecRight.assign( MAX_PLARYER(%u) ) Fail!",
      MAX_PLAYER);
    return false;
  }

  for (size_t index = 0; index < m_vecRight.size(); ++index)
  {
    m_vecRight[index] = baseInfo;
  }
  return true;
}

CMoveMapLimitRightInfo *CMoveMapLimitRightInfoList::Get(int iInx)
{
  if (iInx >= 0 && m_vecRight.size() > static_cast<size_t>(iInx))
  {
    return &m_vecRight[static_cast<size_t>(iInx)];
  }
  return nullptr;
}

void CMoveMapLimitRightInfoList::Load(CPlayer *pkPlayer)
{
  if (pkPlayer && m_vecRight.size() > pkPlayer->m_ObjID.m_wIndex)
  {
    m_vecRight[pkPlayer->m_ObjID.m_wIndex].Load(pkPlayer);
  }
}

void CMoveMapLimitRightInfoList::LogIn(CPlayer *pkPlayer)
{
  if (pkPlayer && m_vecRight.size() > pkPlayer->m_ObjID.m_wIndex)
  {
    m_vecRight[pkPlayer->m_ObjID.m_wIndex].LogIn(pkPlayer);
  }
}

void CMoveMapLimitRightInfoList::LogOut(CPlayer *pkPlayer)
{
  if (pkPlayer && m_vecRight.size() > pkPlayer->m_ObjID.m_wIndex)
  {
    m_vecRight[pkPlayer->m_ObjID.m_wIndex].LogOut(pkPlayer);
  }
}

void CMoveMapLimitRightInfoList::CreateComplete(CPlayer *pkPlayer)
{
  if (pkPlayer && m_vecRight.size() > pkPlayer->m_ObjID.m_wIndex)
  {
    m_vecRight[pkPlayer->m_ObjID.m_wIndex].CreateComplete(pkPlayer);
  }
}

