#pragma once

#include "IdaCompat.h"
#include "CGameObject.h"
#include "CPlayer.h"

/* 3058 */
class __cppobj __declspec(align(8)) CItemBox : public CGameObject
{
  unsigned int m_dwOwnerSerial;
  unsigned __int16 m_wOwnerIndex;
  unsigned int m_dwThrowerSerial;
  unsigned __int8 m_byThrowerID;
  unsigned __int16 m_wThrowerIndex;
  unsigned __int16 m_wMonRecIndex;
  bool m_bBossMob;
  char m_wszThrowerName[17];
  char m_aszThrowerName[17];
  unsigned int m_dwThrowerCharSerial;
  char m_szThrowerID[13];
  unsigned __int8 m_byThrowerRaceCode;
  unsigned __int8 m_byThrowerDegree;
  char *m_szThrowerItemHistoryFileName;
  unsigned int m_dwLootStartTime;
  int m_nStateCode;
  unsigned int m_dwLastDestroyTime;
  unsigned __int8 m_byCreateCode;
  unsigned int m_dwPartyBossSerial;
  bool m_bPartyShare;
  bool m_bCompDgr;
  unsigned int m_dwEventPartyBoss;
  unsigned int m_dwEventGuildSerial;
  unsigned __int8 m_byEventRaceCode;
  unsigned __int8 m_byEventLootAuth;
  int m_bHolyScanner;
  _STORAGE_LIST::_db_con m_Item;
  bool m_bHide;
};
