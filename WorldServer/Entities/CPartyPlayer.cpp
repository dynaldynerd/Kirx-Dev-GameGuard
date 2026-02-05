#include "pch.h"

#include "CPartyPlayer.h"
#include "CDarkHole.h"
#include "GlobalObjects.h"

void CPartyPlayer::Init(unsigned int index)
{
  m_bLogin = false;
  m_id.wIndex = static_cast<unsigned __int16>(index);
  PartyListInit();
  m_pDarkHole = nullptr;
}

void CPartyPlayer::PartyListInit()
{
  m_pPartyBoss = nullptr;
  for (int j = 0; j < 8; ++j)
  {
    m_pPartyMember[j] = nullptr;
  }
  m_bLock = false;
}

bool CPartyPlayer::IsPartyMode()
{
  return m_pPartyBoss != nullptr;
}

bool CPartyPlayer::IsPartyBoss()
{
  return m_pPartyBoss == this;
}

bool CPartyPlayer::IsPartyLock()
{
  return m_pPartyBoss && m_pPartyBoss->m_bLock;
}

CPartyPlayer **CPartyPlayer::GetPtrPartyMember()
{
  if (m_pPartyBoss)
  {
    return m_pPartyBoss->m_pPartyMember;
  }
  return nullptr;
}

unsigned int CPartyPlayer::GetPopPartyMember()
{
  if (!IsPartyMode())
  {
    return 0;
  }

  unsigned int count = 0;
  for (int index = 0; index < 8 && m_pPartyBoss->m_pPartyMember[index]; ++index)
  {
    ++count;
  }
  return count;
}

bool CPartyPlayer::DisjointParty()
{
  if (!IsPartyBoss())
  {
    return false;
  }

  if (m_pDarkHole)
  {
    m_pDarkHole->Destroy();
    m_pDarkHole = nullptr;
  }

  for (int index = 0; index < 8 && m_pPartyMember[index]; ++index)
  {
    CPlayer *memberPlayer = &g_Player[m_pPartyMember[index]->m_wZoneIndex];
    if (memberPlayer->m_bOper)
    {
      if (memberPlayer->GetGroupTarget(0)->pObject)
      {
        memberPlayer->SendMsg_ReleaseGroupTargetObjectResult(0);
      }
    }
  }

  for (int index = 1; index < 8 && m_pPartyMember[index]; ++index)
  {
    m_pPartyMember[index]->PartyListInit();
  }
  PartyListInit();
  return true;
}

bool CPartyPlayer::RemovePartyMember(CPartyPlayer *pExiter, CPartyPlayer **ppoutNewBoss)
{
  if (!IsPartyBoss())
  {
    return false;
  }
  if (pExiter->m_pPartyBoss != this)
  {
    return false;
  }

  bool memberFound = false;
  for (int index = 0; index < 8; ++index)
  {
    if (m_pPartyMember[index] == pExiter)
    {
      memberFound = true;
      break;
    }
  }
  if (!memberFound)
  {
    return false;
  }

  const int popCount = static_cast<int>(GetPopPartyMember());
  if (popCount > 2)
  {
    CPartyPlayer *newBoss = this;
    if (m_pLootAuthor == pExiter)
    {
      for (int index = 0; index < 8 && m_pPartyMember[index]; ++index)
      {
        if (m_pLootAuthor == m_pPartyMember[index])
        {
          if (index == 7)
          {
            m_pLootAuthor = m_pPartyMember[0];
          }
          else if (m_pPartyMember[index + 1])
          {
            m_pLootAuthor = m_pPartyMember[index + 1];
          }
          else
          {
            m_pLootAuthor = m_pPartyMember[0];
          }
          break;
        }
      }
    }

    if (this == pExiter)
    {
      newBoss = m_pPartyMember[1];
      newBoss->PartyListInit();
      newBoss->m_pPartyBoss = newBoss;
      for (int index = 1; index < 8; ++index)
      {
        if (m_pPartyMember[index])
        {
          newBoss->m_pPartyMember[index - 1] = m_pPartyMember[index];
          newBoss->m_pPartyMember[index - 1]->m_pPartyBoss = newBoss;
        }
      }
      for (int index = popCount - 1; index < 8; ++index)
      {
        newBoss->m_pPartyMember[index] = nullptr;
      }
      for (int index = 0; index < 8 && newBoss->m_pPartyMember[index]; ++index)
      {
        CPlayer *memberPlayer = &g_Player[newBoss->m_pPartyMember[index]->m_wZoneIndex];
        if (memberPlayer->m_bOper && memberPlayer->GetGroupTarget(0)->pObject)
        {
          memberPlayer->SendMsg_ReleaseGroupTargetObjectResult(0);
        }
      }
    }
    else
    {
      bool removed = false;
      for (int index = 1; index < 8; ++index)
      {
        if (m_pPartyMember[index] == pExiter)
        {
          removed = true;
          for (int moveIndex = index + 1; moveIndex < 8; ++moveIndex)
          {
            m_pPartyMember[moveIndex - 1] = m_pPartyMember[moveIndex];
          }
          break;
        }
        if (!m_pPartyMember[index])
        {
          break;
        }
      }
      if (removed)
      {
        for (int index = popCount - 1; index < 8; ++index)
        {
          m_pPartyMember[index] = nullptr;
        }
      }
      CPlayer *exitPlayer = &g_Player[pExiter->m_wZoneIndex];
      if (exitPlayer->m_bOper && exitPlayer->GetGroupTarget(0)->pObject)
      {
        exitPlayer->SendMsg_ReleaseGroupTargetObjectResult(0);
      }
    }

    pExiter->PartyListInit();
    if (ppoutNewBoss)
    {
      *ppoutNewBoss = newBoss;
    }
    return true;
  }

  DisjointParty();
  if (ppoutNewBoss)
  {
    *ppoutNewBoss = nullptr;
  }
  return true;
}

