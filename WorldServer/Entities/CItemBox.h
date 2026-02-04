#pragma once

#include "IdaCompat.h"
#include "CGameObject.h"
#include "CPlayer.h"
#include "ObjectCreateSetData.h"

struct __cppobj __declspec(align(2)) _itembox_create_setdata : _object_create_setdata
{
  CPlayer *pOwner;
  CCharacter *pThrower;
  unsigned __int8 byCreateCode;
  bool bParty;
  unsigned int dwPartyBossSerial;
  CPlayer *pAttacker;
  int bHolyScanner;
  unsigned __int8 byEventItemLootAuth;
  __unaligned __declspec(align(1)) _STORAGE_LIST::_db_con Item;

  _itembox_create_setdata()
  {
    pOwner = nullptr;
    pThrower = nullptr;
    byCreateCode = 0;
    bParty = false;
    dwPartyBossSerial = static_cast<unsigned int>(-1);
    pAttacker = nullptr;
    bHolyScanner = 0;
    byEventItemLootAuth = 3;
  }
};

/* 3058 */
class __cppobj __declspec(align(8)) CItemBox : public CGameObject
{
public:
  static int s_nLiveNum;
  static unsigned int s_dwSerialCounter;

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

  CItemBox();
  virtual ~CItemBox();
  void Init(_object_id *pID);
  bool Create(_itembox_create_setdata *pParam, bool bHide);
  bool Destroy();
  void SendMsg_Create();
  void SendMsg_Destroy();
  void SendMsg_StateChange();
  void SendMsg_FixPosition(unsigned int n);
};

CItemBox *CreateItemBox(
  _STORAGE_LIST::_db_con *pItem,
  CPlayer *pOwner,
  unsigned int dwPartyBossSerial,
  bool bPartyShare,
  CCharacter *pThrower,
  unsigned __int8 byCreateCode,
  CMapData *pMap,
  unsigned __int16 wLayerIndex,
  float *pStdPos,
  bool bHide);

CItemBox *CreateItemBox(
  _STORAGE_LIST::_db_con *pItem,
  unsigned __int8 byCreateCode,
  CMapData *pMap,
  unsigned __int16 wLayerIndex,
  float *pStdPos,
  bool bHide,
  CPlayer *pAttacker,
  int bHolyScanner,
  unsigned __int8 byEventItemLootAuth);
