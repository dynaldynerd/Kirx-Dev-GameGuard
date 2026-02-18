#pragma once

#include "IdaCompat.h"
#include "CPlayer.h"

/* 1597 */
class  CPartyPlayer
{
public:
  CPartyPlayer();

  bool m_bLogin;
   _CLID m_id;
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
  bool IsJoinPartyLevel(int joinerLevel, float profileBonus);
  CPartyPlayer **GetPtrPartyMember();
  CPartyPlayer *GetPtrFromSerial(unsigned int worldSerial);
  CPlayer *GetLootAuthor();
  void SetNextLootAuthor();
  unsigned int GetPopPartyMember();
  bool FoundParty(CPartyPlayer *participant);
  bool InsertPartyMember(CPartyPlayer *joiner);
  bool SetLockMode(bool lock);
  bool SetLootShareMode(unsigned __int8 lootShareMode);
  bool InheritBoss(CPartyPlayer *successor);
  bool DisjointParty();
  bool RemovePartyMember(CPartyPlayer *pExiter, CPartyPlayer **ppoutNewBoss);
  void EnterWorld(_WA_AVATOR_CODE *pData, unsigned __int16 wZoneIndex);
  void ExitWorld(CPartyPlayer **ppoutNewBoss);
};

