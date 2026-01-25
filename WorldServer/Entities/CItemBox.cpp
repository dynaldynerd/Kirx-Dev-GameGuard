#include "pch.h"

#include "CItemBox.h"

#include <cstring>

void CItemBox::Init(_object_id *pID)
{
  CGameObject::Init(pID);
  m_dwLastDestroyTime = 0;
  m_wMonRecIndex = static_cast<unsigned __int16>(-1);
  m_bBossMob = false;
  std::memset(m_wszThrowerName, 0, sizeof(m_wszThrowerName));
  std::memset(m_aszThrowerName, 0, sizeof(m_aszThrowerName));
  std::memset(m_szThrowerID, 0, sizeof(m_szThrowerID));
  if (m_szThrowerItemHistoryFileName != nullptr)
  {
    std::memset(m_szThrowerItemHistoryFileName, 0, 0x40);
  }
  m_byCreateCode = static_cast<unsigned __int8>(-1);
  m_bCompDgr = false;
  m_bHide = false;
}
