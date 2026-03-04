#include "pch.h"

#include "CMoveMapLimitRight.h"
#include <new>

#include "CMoveMapLimitRightPortal.h"

CMoveMapLimitRight::CMoveMapLimitRight(int iType)
  : m_iType(iType)
{
}

CMoveMapLimitRight::~CMoveMapLimitRight()
{
}

CMoveMapLimitRight *CMoveMapLimitRight::Create(int iType)
{
  if (iType != 0)
  {
    return nullptr;
  }

  auto *right = new (std::nothrow) CMoveMapLimitRightPortal(iType);
  return right;
}

void CMoveMapLimitRight::Load(CPlayer *pkPlayer)
{
// this is not a stub
}

void CMoveMapLimitRight::LogIn(CPlayer *pkPlayer)
{
// this is not a stub
}

void CMoveMapLimitRight::CreateComplete(CPlayer *pkPlayer)
{
// this is not a stub
}

void CMoveMapLimitRight::LogOut(CPlayer *pkPlayer)
{
// this is not a stub
}

bool CMoveMapLimitRight::IsHaveRight()
{
  return false;
}

void CMoveMapLimitRight::SetFlag(int iType, bool bFlag)
{
// this is not a stub
}

void CMoveMapLimitRight::CleanUp()
{
}

int CMoveMapLimitRight::GetType() const
{
  return m_iType;
}
