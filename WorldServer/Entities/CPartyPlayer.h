#pragma once

#include "IdaCompat.h"
#include "CPlayer.h"

/* 1597 */
class __cppobj CPartyPlayer
{
  bool m_bLogin;
  __unaligned __declspec(align(1)) _CLID m_id;
  char m_wszName[17];
  unsigned __int16 m_wZoneIndex;
  CPartyPlayer *m_pPartyBoss;
  CPartyPlayer *m_pPartyMember[8];
  bool m_bLock;
  unsigned __int8 m_byLootShareSystem;
  CPartyPlayer *m_pLootAuthor;
  CDarkHole *m_pDarkHole;
};

