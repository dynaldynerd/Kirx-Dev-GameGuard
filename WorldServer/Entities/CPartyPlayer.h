#pragma once

#include "IdaCompat.h"
#include "CPlayer.h"

/* 1597 */
class __cppobj CPartyPlayer
{
public:
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

  void Init(unsigned int index);
  void PartyListInit();
  bool IsPartyMode();
  bool IsPartyBoss();
  bool IsPartyLock();
  bool IsPartyMember(CPlayer *pkObj);
  CPartyPlayer **GetPtrPartyMember();
  CPartyPlayer *GetPtrFromSerial(unsigned int dwWorldSerial);
  bool InsertPartyMember(CPartyPlayer *pJoiner);
  bool FoundParty(CPartyPlayer *pParticiper);
  bool SetLockMode(bool bLock);
  bool SetLootShareMode(unsigned __int8 byLootShareMode);
  CPlayer *GetLootAuthor();
  void SetNextLootAuthor();
  unsigned int GetPopPartyMember();
  bool IsJoinPartyLevel(int nJoinerLevel, float fProf);
  bool DisjointParty();
  bool RemovePartyMember(CPartyPlayer *pExiter, CPartyPlayer **ppoutNewBoss);
  bool InheritBoss(CPartyPlayer *pSuccessor);
  void ExitWorld(CPartyPlayer **ppoutNewBoss);
};

