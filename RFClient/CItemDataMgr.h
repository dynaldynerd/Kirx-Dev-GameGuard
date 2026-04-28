#pragma once

#include <windows.h>

#include <vector>

#include "EdfFile.h"

enum RF_ITEM_POSSESSION_TYPE
{
  IEPT_FACE = 0,
  IEPT_UPPER_PART = 1,
  IEPT_LOWER_PART = 2,
  IEPT_GLOVES = 3,
  IEPT_SHOES = 4,
  IEPT_HELMET = 5,
  IEPT_WEAPON = 6,
  IEPT_SHIELD = 7,
  IEPT_CLOAK = 8,
  IEPT_RING = 9,
  IEPT_AMULET = 10,
  IEPT_BULLET = 11,
  IEPT_WORKTOOL = 12,
  IEPT_POTION = 13,
  IEPT_ETC = 14,
  IEPT_BATTERY = 15,
  IEPT_ORE = 16,
  IEPT_INGOT = 17,
  IEPT_FORCE = 18,
  IEPT_UNIT = 19,
  IEPT_LOOT = 20,
  IEPT_MAP = 21,
  IEPT_RETURN = 22,
  IEPT_DUNGEON = 23,
  IEPT_ANIMUS = 24,
  IEPT_GUARD_TOWER = 25,
  IEPT_UNIT_HELMET = 26,
  IEPT_UNIT_UPPER_PART = 27,
  IEPT_UNIT_LOWER_PART = 28,
  IEPT_UNIT_ARMS = 29,
  IEPT_UNIT_SHOULDER = 30,
  IEPT_UNIT_BACKPACK = 31,
  IEPT_UNIT_BULLET = 32,
  IEPT_ITEM_WORK = 33
};

struct ITEM_CLIENT_RECORD
{
  DWORD dwDTIndex;
  DWORD dwDTCode;
  DWORD dwMeshID;
  DWORD dwIconID;
  BYTE byItemType;
};

class CItemDataMgr
{
public:
  CItemDataMgr();
  ~CItemDataMgr();

  BOOL Create(void);
  BOOL Destroy(void);

  BOOL LoadData(void);
  void UnloadData(void);
  BOOL IsLoaded(void) const;
  DWORD GetRecordCount(BYTE pi_byItemType) const;
  const ITEM_CLIENT_RECORD *GetClientRecord(BYTE pi_byItemType, DWORD pi_dwDTIndex) const;
  DWORD GetClientMeshID(BYTE pi_byItemType, DWORD pi_dwDTIndex) const;

private:
  BOOL ParseItemData(void);

private:
  enum { CLIENT_ITEM_SECTION_NUM = 47 };

  BOOL m_bCreated;
  BOOL m_bLoaded;
  std::vector<ITEM_CLIENT_RECORD> m_vecItemRecords[CLIENT_ITEM_SECTION_NUM];

  CEdfFile m_cItemData;
  CEdfFile m_cItemCashData;
  CEdfFile m_cItemCombineData;
  CEdfFile m_cItemPremiumData;
  CEdfFile m_cItemSetEffectData;
  CEdfFile m_cItemTimerData;

  CEdfFile m_cNationItemData;
  CEdfFile m_cNationItemCashData;
};

extern CItemDataMgr *g_pItemDataMgr;

#define _GetItemDataMgr() g_pItemDataMgr
