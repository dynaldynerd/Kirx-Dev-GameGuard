#include "pch.h"

#include "CPartyPlayer.h"

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

