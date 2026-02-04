#include "pch.h"

#include "CMoveMapLimitRightInfo.h"

#include <algorithm>

#include "CMoveMapLimitRight.h"

CMoveMapLimitRightInfo::CMoveMapLimitRightInfo() = default;

CMoveMapLimitRightInfo::CMoveMapLimitRightInfo(const CMoveMapLimitRightInfo &rhs)
  : m_vecRight(rhs.m_vecRight)
{
}

CMoveMapLimitRightInfo::~CMoveMapLimitRightInfo()
{
  CleanUp();
}

CMoveMapLimitRightInfo &CMoveMapLimitRightInfo::operator=(const CMoveMapLimitRightInfo &rhs)
{
  if (this == &rhs)
  {
    return *this;
  }

  CleanUp();
  for (CMoveMapLimitRight *right : rhs.m_vecRight)
  {
    const int type = right->GetType();
    CMoveMapLimitRight *created = CMoveMapLimitRight::Create(type);
    m_vecRight.push_back(created);
  }
  return *this;
}

bool CMoveMapLimitRightInfo::Regist(int iType)
{
  CMoveMapLimitRight *created = CMoveMapLimitRight::Create(iType);
  if (!created)
  {
    return false;
  }

  if (std::find(m_vecRight.begin(), m_vecRight.end(), created) != m_vecRight.end())
  {
    return false;
  }

  m_vecRight.push_back(created);
  return true;
}

void CMoveMapLimitRightInfo::Load(CPlayer *pkPlayer)
{
  for (CMoveMapLimitRight *right : m_vecRight)
  {
    right->Load(pkPlayer);
  }
}

void CMoveMapLimitRightInfo::LogIn(CPlayer *pkPlayer)
{
  for (CMoveMapLimitRight *right : m_vecRight)
  {
    right->LogIn(pkPlayer);
  }
}

void CMoveMapLimitRightInfo::LogOut(CPlayer *pkPlayer)
{
  for (CMoveMapLimitRight *right : m_vecRight)
  {
    right->LogOut(pkPlayer);
  }
}

void CMoveMapLimitRightInfo::CreateComplete(CPlayer *pkPlayer)
{
  for (CMoveMapLimitRight *right : m_vecRight)
  {
    right->CreateComplete(pkPlayer);
  }
}

bool CMoveMapLimitRightInfo::IsHaveRight(int iType)
{
  if (iType < 0 || m_vecRight.empty() || m_vecRight.size() <= static_cast<size_t>(iType) || !m_vecRight[iType])
  {
    return true;
  }
  return m_vecRight[iType]->IsHaveRight();
}

void CMoveMapLimitRightInfo::SetFlag(int iType, unsigned int iSubType, bool bFlag)
{
  if (iType >= 0 && !m_vecRight.empty() && m_vecRight.size() > static_cast<size_t>(iType))
  {
    if (m_vecRight[iType])
    {
      m_vecRight[iType]->SetFlag(static_cast<int>(iSubType), bFlag);
    }
  }
}

void CMoveMapLimitRightInfo::CleanUp()
{
  for (CMoveMapLimitRight *right : m_vecRight)
  {
    if (right)
    {
      right->CleanUp();
      delete right;
    }
  }
  m_vecRight.clear();
}

