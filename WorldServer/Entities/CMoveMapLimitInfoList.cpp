#include "pch.h"

#include "CMoveMapLimitInfoList.h"

#include <algorithm>
#include <cstdio>

#include "CMoveMapLimitInfo.h"
#include "CMainThread.h"

bool CMoveMapLimitInfoList::Init(std::vector<int> *vecRightTypeList)
{
  const unsigned int count =
    static_cast<unsigned int>(GetPrivateProfileIntA("MoveMapLimitInfo", "Cnt", 0, "./Initialize/MoveMapLimit.ini"));
  if (count == 0)
  {
    return true;
  }

  CMoveMapLimitInfo *nullInfo = nullptr;
  m_vecLimitInfo.assign(count, nullInfo);
  if (m_vecLimitInfo.size() != count)
  {
    g_Main.m_logLoadingError.Write(
      "CMoveMapLimitInfoList::Init() : uiCnt(%u) != m_vecLimitInfo.size()(%d)",
      count,
      static_cast<int>(m_vecLimitInfo.size()));
    return false;
  }

  for (unsigned int index = 0; index < count; ++index)
  {
    char key[1024]{};
    std::sprintf(key, "Type%u", index);
    const int limitType =
      GetPrivateProfileIntA("MoveMapLimitInfo", key, -1, "./Initialize/MoveMapLimit.ini");
    if (limitType == -1)
    {
      g_Main.m_logLoadingError.Write(
        "CMoveMapLimitInfoList::Init() : [%s] Type%u == -1!",
        "MoveMapLimitInfo",
        index);
      return false;
    }

    if (std::find(vecRightTypeList->begin(), vecRightTypeList->end(), limitType) == vecRightTypeList->end())
    {
      vecRightTypeList->push_back(limitType);
    }

    CMoveMapLimitInfo *info = CMoveMapLimitInfo::Create(index, limitType);
    if (!info)
    {
      g_Main.m_logLoadingError.Write(
        "CMoveMapLimitInfoList::Init() : CMoveMapLimitInfo::Create( i(%u), iType(%d) ) NULL!",
        index,
        limitType);
      return false;
    }

    if (!info->Init())
    {
      g_Main.m_logLoadingError.Write(
        "CMoveMapLimitInfoList::Init() : %uth Type(%d) pkInfo->Init() Fail!",
        index,
        limitType);
      return false;
    }
    m_vecLimitInfo[index] = info;
  }

  return true;
}

CMoveMapLimitInfo *CMoveMapLimitInfoList::Get(int iLimitType, int iMapInx, unsigned int dwStoreRecordIndex)
{
  for (CMoveMapLimitInfo *info : m_vecLimitInfo)
  {
    if (info && info->IsEqualLimit(iLimitType, iMapInx, dwStoreRecordIndex))
    {
      return info;
    }
  }
  return nullptr;
}

void CMoveMapLimitInfoList::Load(CPlayer *pkPlayer, CMoveMapLimitRightInfo *pkRight)
{
  for (CMoveMapLimitInfo *info : m_vecLimitInfo)
  {
    info->Load(pkPlayer, pkRight);
  }
}

void CMoveMapLimitInfoList::LogIn(CPlayer *pkPlayer, CMoveMapLimitRightInfo *pkRight)
{
  for (CMoveMapLimitInfo *info : m_vecLimitInfo)
  {
    info->LogIn(pkPlayer, pkRight);
  }
}

void CMoveMapLimitInfoList::LogOut(CPlayer *pkPlayer, CMoveMapLimitRightInfo *pkRight)
{
  for (CMoveMapLimitInfo *info : m_vecLimitInfo)
  {
    info->LogOut(pkPlayer, pkRight);
  }
}

unsigned __int8 CMoveMapLimitInfoList::Request(
  int iLimitType,
  unsigned int iRequetType,
  int iMapInx,
  unsigned int dwStoreRecordIndex,
  unsigned int iUserInx,
  char *pRequest,
  CMoveMapLimitRightInfo *pkRight)
{
  CMoveMapLimitInfo *info = Get(iLimitType, iMapInx, dwStoreRecordIndex);
  if (info)
  {
    return info->Request(static_cast<int>(iUserInx), static_cast<int>(iRequetType), pRequest, pkRight);
  }
  return 3;
}

