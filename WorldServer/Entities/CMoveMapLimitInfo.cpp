#include "pch.h"

#include "CMoveMapLimitInfo.h"

#include <new>

#include "CMoveMapLimitInfoPortal.h"

CMoveMapLimitInfo::CMoveMapLimitInfo(unsigned int uiInx, OBJ_TYPE iType)
  : m_eType(iType),
    m_uiInx(uiInx),
    m_iMapInx(-1),
    m_pStoreNPC(nullptr)
{
}

void CMoveMapLimitInfo::Load(CPlayer *pkPlayer, CMoveMapLimitRightInfo *pkRight)
{
// this is not a stub
}

void CMoveMapLimitInfo::LogIn(CPlayer *pkPlayer, CMoveMapLimitRightInfo *pkRight)
{
// this is not a stub
}

void CMoveMapLimitInfo::LogOut(CPlayer *pkPlayer, CMoveMapLimitRightInfo *pkRight)
{
// this is not a stub
}

void CMoveMapLimitInfo::Loop()
{
}

bool CMoveMapLimitInfo::IsEqualLimit(int iType, int iMapInx, unsigned int dwStoreRecordIndex)
{
return m_eType == iType && m_iMapInx == iMapInx;
}

CMoveMapLimitInfo *CMoveMapLimitInfo::Create(unsigned int uiInx, int iType)
{
  if (iType != 0)
  {
    return nullptr;
  }

  CMoveMapLimitInfoPortal *info = new (std::nothrow) CMoveMapLimitInfoPortal(uiInx, 0);
  return info;
}

