#include "pch.h"

#include "CMoveMapLimitRight.h"

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

  auto *right = new CMoveMapLimitRightPortal(iType);
  return right;
}

void CMoveMapLimitRight::Load(CPlayer *pkPlayer)
{
  (void)pkPlayer;
}

void CMoveMapLimitRight::LogIn(CPlayer *pkPlayer)
{
  (void)pkPlayer;
}

void CMoveMapLimitRight::CreateComplete(CPlayer *pkPlayer)
{
  (void)pkPlayer;
}

void CMoveMapLimitRight::LogOut(CPlayer *pkPlayer)
{
  (void)pkPlayer;
}

bool CMoveMapLimitRight::IsHaveRight()
{
  return false;
}

void CMoveMapLimitRight::SetFlag(int iType, bool bFlag)
{
  (void)iType;
  (void)bFlag;
}

void CMoveMapLimitRight::CleanUp()
{
}

int CMoveMapLimitRight::GetType() const
{
  return m_iType;
}

