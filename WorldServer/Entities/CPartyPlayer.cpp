#include "pch.h"

#include "CPartyPlayer.h"
#include "CDarkHole.h"
#include "GlobalObjects.h"
#include <cstdlib>

CPartyPlayer::CPartyPlayer()
  : m_bLogin(false)
{
}

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

bool CPartyPlayer::IsPartyMember(CPlayer *pkObj)
{
  CPartyPlayer **members = GetPtrPartyMember();
  if (!members)
  {
    return false;
  }

  for (int index = 0; index < 8 && members[index]; ++index)
  {
    if (members[index]->m_id.dwSerial == pkObj->m_dwObjSerial)
    {
      return true;
    }
  }

  return false;
}

bool CPartyPlayer::IsJoinPartyLevel(int joinerLevel, float profileBonus)
{
  if (IsPartyMode())
  {
    unsigned __int8 minLevel = static_cast<unsigned __int8>(-1);
    unsigned __int8 maxLevel = 0;
    CPartyPlayer **members = GetPtrPartyMember();

    for (int index = 0; index < 8 && members[index]; ++index)
    {
      CPlayer *partyMember = &g_Player[members[index]->m_wZoneIndex];
      const int memberLevel = static_cast<int>(partyMember->GetLevel());
      if (minLevel > memberLevel)
      {
        minLevel = static_cast<unsigned __int8>(memberLevel);
      }
      if (maxLevel < memberLevel)
      {
        maxLevel = static_cast<unsigned __int8>(memberLevel);
      }
    }

    if (minLevel == static_cast<unsigned __int8>(-1) || maxLevel == 0 || minLevel > maxLevel)
    {
      return false;
    }
    if (joinerLevel > maxLevel && static_cast<float>(joinerLevel - minLevel) > (profileBonus + 10.0f))
    {
      return false;
    }
    if (joinerLevel < minLevel && static_cast<float>(maxLevel - joinerLevel) > (profileBonus + 10.0f))
    {
      return false;
    }
  }
  else
  {
    CPlayer *singleMember = &g_Player[m_wZoneIndex];
    const int levelDiff = std::abs(static_cast<int>(singleMember->GetLevel()) - joinerLevel);
    if (static_cast<float>(levelDiff) > (profileBonus + 10.0f))
    {
      return false;
    }
  }

  return true;
}

CPartyPlayer **CPartyPlayer::GetPtrPartyMember()
{
  if (m_pPartyBoss)
  {
    return m_pPartyBoss->m_pPartyMember;
  }
  return nullptr;
}

CPartyPlayer *CPartyPlayer::GetPtrFromSerial(unsigned int worldSerial)
{
  if (!m_pPartyBoss)
  {
    return nullptr;
  }

  for (int index = 0; index < 8; ++index)
  {
    CPartyPlayer *member = m_pPartyBoss->m_pPartyMember[index];
    if (!member)
    {
      return nullptr;
    }
    if (member->m_id.dwSerial == worldSerial)
    {
      return member;
    }
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

bool CPartyPlayer::FoundParty(CPartyPlayer *participant)
{
  if (IsPartyMode())
  {
    return false;
  }
  if (participant->IsPartyMode())
  {
    return false;
  }

  PartyListInit();
  m_pPartyBoss = this;
  m_pPartyMember[0] = this;
  m_byLootShareSystem = 0;
  m_pLootAuthor = this;
  InsertPartyMember(participant);
  return true;
}

bool CPartyPlayer::InsertPartyMember(CPartyPlayer *joiner)
{
  if (!IsPartyBoss())
  {
    return false;
  }

  for (int index = 0; index < 8; ++index)
  {
    if (!m_pPartyMember[index])
    {
      m_pPartyMember[index] = joiner;
      joiner->m_pPartyBoss = this;
      return true;
    }
  }

  return false;
}

bool CPartyPlayer::SetLockMode(bool lock)
{
  if (!IsPartyBoss())
  {
    return false;
  }
  if (m_bLock == lock)
  {
    return false;
  }

  m_bLock = lock;
  return true;
}

bool CPartyPlayer::SetLootShareMode(unsigned __int8 lootShareMode)
{
  if (!IsPartyBoss())
  {
    return false;
  }
  if (m_byLootShareSystem == lootShareMode)
  {
    return false;
  }

  m_byLootShareSystem = lootShareMode;
  return true;
}

bool CPartyPlayer::InheritBoss(CPartyPlayer *successor)
{
  if (!IsPartyBoss())
  {
    return false;
  }
  if (successor->m_pPartyBoss != this)
  {
    return false;
  }

  if (m_pLootAuthor == successor)
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

  bool replaced = false;
  for (int index = 0; index < 8; ++index)
  {
    if (m_pPartyMember[index] == successor)
    {
      CPartyPlayer *swapMember = m_pPartyMember[0];
      for (int shift = 0; shift < index; ++shift)
      {
        CPartyPlayer *nextMember = swapMember;
        swapMember = m_pPartyMember[shift + 1];
        m_pPartyMember[shift + 1] = nextMember;
      }
      m_pPartyMember[0] = successor;
      replaced = true;
      break;
    }
  }
  if (!replaced)
  {
    return false;
  }

  for (int index = 0; index < 8; ++index)
  {
    successor->m_pPartyMember[index] = m_pPartyMember[index];
  }
  successor->m_byLootShareSystem = m_byLootShareSystem;
  successor->m_pLootAuthor = m_pLootAuthor;

  for (int index = 0; index < 8 && successor->m_pPartyMember[index]; ++index)
  {
    successor->m_pPartyMember[index]->m_pPartyBoss = successor;
  }

  for (int index = 0; index < 8 && successor->m_pPartyMember[index]; ++index)
  {
    CPlayer *memberPlayer = &g_Player[successor->m_pPartyMember[index]->m_wZoneIndex];
    if (memberPlayer->m_bOper && memberPlayer->GetGroupTarget(0)->pObject)
    {
      memberPlayer->SendMsg_ReleaseGroupTargetObjectResult(0);
    }
  }

  return true;
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

void CPartyPlayer::ExitWorld(CPartyPlayer **ppoutNewBoss)
{
  m_bLogin = false;
  m_id.dwSerial = static_cast<unsigned int>(-1);
  if (IsPartyMode())
  {
    m_pPartyBoss->RemovePartyMember(this, ppoutNewBoss);
  }
}

void CPartyPlayer::EnterWorld(_WA_AVATOR_CODE *pData, unsigned __int16 wZoneIndex)
{
  this->m_bLogin = true;
  this->m_wZoneIndex = wZoneIndex;
  memcpy_0(&this->m_id, pData, sizeof(this->m_id));
  strcpy_0(this->m_wszName, pData->m_wszName);
  this->PartyListInit();
}

CPlayer *CPartyPlayer::GetLootAuthor()
{
  if (!IsPartyMode())
  {
    return nullptr;
  }

  CPlayer *lootAuthor = nullptr;
  const unsigned __int8 lootShareSystem = m_pPartyBoss->m_byLootShareSystem;
  switch (lootShareSystem)
  {
    case 0:
      return nullptr;
    case 1:
      return &g_Player[m_pPartyBoss->m_wZoneIndex];
    case 2:
      if (m_pPartyBoss->m_pLootAuthor)
      {
        return &g_Player[m_pPartyBoss->m_pLootAuthor->m_wZoneIndex];
      }
      lootAuthor = &g_Player[m_pPartyBoss->m_wZoneIndex];
      m_pPartyBoss->m_pLootAuthor = m_pPartyBoss;
      break;
  }
  return lootAuthor;
}

void CPartyPlayer::SetNextLootAuthor()
{
  if (IsPartyMode() && m_pPartyBoss->m_byLootShareSystem == 2)
  {
    CPartyPlayer **members = GetPtrPartyMember();
    for (int j = 0; j < 8; ++j)
    {
      if (members[j] == m_pPartyBoss->m_pLootAuthor)
      {
        if (j == 7)
        {
          m_pPartyBoss->m_pLootAuthor = *members;
        }
        else if (members[j + 1])
        {
          m_pPartyBoss->m_pLootAuthor = members[j + 1];
        }
        else
        {
          m_pPartyBoss->m_pLootAuthor = *members;
        }
        return;
      }
    }
  }
}
