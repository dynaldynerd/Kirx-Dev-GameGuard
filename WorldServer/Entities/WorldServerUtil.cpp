#include "pch.h"

#include "WorldServerUtil.h"
#include "R3EngineStateOverlay.h"
#include "R3TextureSlot.h"
#include "R3XLoadData.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <intrin.h>
#include <atlstr.h>
#include <atltime.h>

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include "CMerchant.h"
#include "CRecordData.h"
#include "CashItemRemoteStore.h"
#include "CItemStore.h"
#include "CItemStoreManager.h"
#include "CMapData.h"
#include "CPlayer.h"
#include "StoreList_fld.h"
#include "GlobalObjectDefs.h"
#include "GlobalObjects.h"
#include "ItemDataLoader.h"
#include "R3EngineState.h"
#include "R3EngineGlobals.h"
#include "CMergeFileManager.h"
#include "base_fld.h"
#include "DfnEquipItem_fld.h"
#include "CloakItem_fld.h"
#include "BagItem_fld.h"
#include "MakeToolItem_fld.h"
#include "BulletItem_fld.h"
#include "PotionItem_fld.h"
#include "ResourceItem_fld.h"
#include "TOWNItem_fld.h"
#include "BoxItem_fld.h"
#include "ForceItem_fld.h"
#include "OreItem_fld.h"
#include "UnitKeyItem_fld.h"
#include "BootyItem_fld.h"
#include "MapItem_fld.h"
#include "BattleDungeonItem_fld.h"
#include "AnimusItem_fld.h"
#include "FaceItem_fld.h"
#include "RecoveryItem_fld.h"
#include "TicketItem_fld.h"
#include "FIRECRACKER_fld.h"
#include "EventItem_fld.h"
#include "RadarItem_fld.h"
#include "NPCLink_fld.h"
#include "CouponItem_fld.h"
#include "BatteryItem_fld.h"
#include "GuardTowerItem_fld.h"
#include "TrapItem_fld.h"
#include "SiegeKitItem_fld.h"
#include "UNmannedminer_fld.h"
#include "RingItem_fld.h"
#include "AmuletItem_fld.h"
#include "EQUIP_MASTERY_LIM.h"
#include "CAnimus.h"
#include "animus_fld.h"
#include "WeaponItem_fld.h"
#include "skill_fld.h"
#include "force_fld.h"

namespace
{
  char szDefCivilCode[] = "11111";
}

const char *dayofweek[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
static char szDefItemName[] = "noname";
static float sR[4] = {200.0f, 200.0f, 200.0f, 200.0f};
static int s_nSkillLvPerMastery[8];
static int s_nForceLvPerMastery[24];
static char sBuffer[64];

static __int64 D3DXCreateTextureFromFileInMemory_0(IDirect3DDevice8 *device, const void *data, unsigned int size, void *outTex);
static __int64 sub_1404FFFB0(__int64 a1);
static __int64 sub_1404FFF30(unsigned int *a1);
static unsigned __int16 sub_1405005A0(unsigned __int16 a1);
static int sub_140501380(char *a1, const char *a2, const char *a3);
static void sub_140502610(_R3MATERIAL *material);
static int sub_140502690(FILE *Stream, _R3MATERIAL *material, int layerIndex);
static void sub_140503190(char *a1, _R3MATERIAL *material);
static void LoadR3X(char *a1);
static _LIGHTMAP **LoadR3TLightMap(struct R3Texture *a1, D3DFORMAT a2);
static void RestoreSystemTexture();
static IDirect3DTexture8 *GetD3DTextureFromBuffer(unsigned __int8 *a1, unsigned int a2);
static int GetMipMapSkipSize(IDA_DDSURFACEDESC2 *a1, unsigned int a2, unsigned int a3, unsigned int a4);
static void MakeMipMap(unsigned short a1, unsigned short a2, unsigned short *a3, unsigned short *a4);
static void MakeMipMap(unsigned short a1, unsigned short a2, unsigned short *a3, unsigned char *a4);

namespace
{
  static_assert(sizeof(R3Texture) == 144);
  static_assert(sizeof(_R3MATERIAL) == 470);
  static_assert(sizeof(_ONE_LAYER) == 46);

  R3Texture *GetTextureTable()
  {
    return static_cast<R3Texture *>(qword_184A79D70);
  }

  R3TextureSlot *GetTextureSlots()
  {
    return static_cast<R3TextureSlot *>(qword_184A79D68);
  }

  unsigned int ReadDwordAt(const unsigned int *data, size_t index)
  {
    return data[index];
  }

  unsigned short ReadWordAt(const unsigned int *data, size_t index)
  {
    return static_cast<const unsigned short *>(static_cast<const void *>(data))[index];
  }
}

unsigned int GetLoopTime()
{
  return timeGetTime();
}

CPlayer *GetPtrPlayerFromName(CPlayer *pData, int nNum, char *pwszName)
{
  const unsigned __int8 nameLength = static_cast<unsigned __int8>(std::strlen(pwszName));
  for (int j = 0; j < nNum; ++j)
  {
    CPlayer *player = &pData[j];
    if (player->m_bLive && player->m_Param.m_byNameLen == nameLength)
    {
      const size_t maxCount = nameLength;
      const char *charName = player->m_Param.GetCharNameW();
      if (!strncmp(charName, pwszName, maxCount))
      {
        return player;
      }
    }
  }
  return nullptr;
}

unsigned long long GetConnectTime_AddBySec(int iSec)
{
  __time64_t timeValue{};
  _time64(&timeValue);
  timeValue += static_cast<__time64_t>(iSec);

  tm local{};
  _localtime64_s(&local, &timeValue);

  char buffer[44]{};
  sprintf_s(
    buffer,
    sizeof(buffer),
    "%01d%02d%02d%02d%02d",
    local.tm_year - 100,
    local.tm_mon + 1,
    local.tm_mday,
    local.tm_hour,
    local.tm_min);
  return std::strtoull(buffer, nullptr, 10);
}

char *cvt_string(int nVal)
{
  _itoa(nVal, sBuffer, 10);
  return sBuffer;
}

bool CanAddMoneyForMaxLimMoney(unsigned __int64 ui64AddMoney, unsigned __int64 ui64HasMoney)
{
  if (ui64AddMoney > MAX_DALANT)
  {
    return false;
  }
  if (ui64HasMoney <= MAX_DALANT)
  {
    return ui64AddMoney <= static_cast<unsigned __int64>(MAX_DALANT) - ui64HasMoney;
  }
  return false;
}

bool CanAddMoneyForMaxLimGold(unsigned __int64 ui64AddGold, unsigned __int64 ui64HasGold)
{
  if (ui64AddGold > MAX_GOLD)
  {
    return false;
  }
  if (ui64HasGold <= MAX_GOLD)
  {
    return ui64AddGold <= static_cast<unsigned __int64>(MAX_GOLD) - ui64HasGold;
  }
  return false;
}

int eGetRate(int nRaceCode)
{
  if (e_EconomySystem.m_CurRate[nRaceCode].fPayExgRate < 1000.0f)
  {
    e_EconomySystem.m_CurRate[nRaceCode].fPayExgRate = 1000.0f;
  }
  if (e_EconomySystem.m_CurRate[nRaceCode].fPayExgRate > 3000.0f)
  {
    e_EconomySystem.m_CurRate[nRaceCode].fPayExgRate = 3000.0f;
  }
  return static_cast<int>(e_EconomySystem.m_CurRate[nRaceCode].fPayExgRate);
}

int eGetMgrValue()
{
  return e_nMgrDft;
}

_economy_history_data *eGetGuideHistory()
{
  return e_EconomyHistory;
}

unsigned __int16 eGetGuide(int nRaceCode)
{
  return e_EconomySystem.m_CurRate[nRaceCode].wEconomyGuide;
}

float eGetTex(int nRaceCode)
{
  if (e_EconomySystem.m_CurRate[nRaceCode].fTexRate < 0.0099999998f)
  {
    e_EconomySystem.m_CurRate[nRaceCode].fTexRate = 0.0099999998f;
  }
  if (e_EconomySystem.m_CurRate[nRaceCode].fTexRate > 0.30000001f)
  {
    e_EconomySystem.m_CurRate[nRaceCode].fTexRate = 0.30000001f;
  }
  return e_EconomySystem.m_CurRate[nRaceCode].fTexRate;
}

unsigned int eGetTexRate(int nRaceCode)
{
  if (e_EconomySystem.m_CurRate[nRaceCode].dwTexRate < 100u)
  {
    e_EconomySystem.m_CurRate[nRaceCode].dwTexRate = 100u;
  }
  if (e_EconomySystem.m_CurRate[nRaceCode].dwTexRate > 3000u)
  {
    e_EconomySystem.m_CurRate[nRaceCode].dwTexRate = 3000u;
  }
  return e_EconomySystem.m_CurRate[nRaceCode].dwTexRate;
}

float eGetOreRate(int nRaceCode)
{
  return e_EconomySystem.m_CurRate[nRaceCode].fOreRate;
}

void eAddDalant(int nRaceCode, int nAdd)
{
  e_EconomySystem.m_dBufTradeDalant[nRaceCode] += static_cast<double>(nAdd);
  if (e_EconomySystem.m_dBufTradeDalant[nRaceCode] < 0.0)
  {
    e_EconomySystem.m_dBufTradeDalant[nRaceCode] = 0.0;
  }
}

void eAddGold(int nRaceCode, int nAdd)
{
  e_EconomySystem.m_dBufTradeGold[nRaceCode] += static_cast<double>(nAdd);
}

void eAddMineOre(int nRaceCode, unsigned __int8 byKind, int nAdd)
{
  e_EconomySystem.m_dBufOreMineCount[nRaceCode][byKind] += static_cast<double>(nAdd);
  if (e_EconomySystem.m_dBufOreMineCount[nRaceCode][byKind] < 0.0)
  {
    e_EconomySystem.m_dBufOreMineCount[nRaceCode][byKind] = 0.0;
  }
}

CItemStore *IsBeNearStore(CPlayer *p, int nStoreCode)
{
  CMapItemStoreList *storeList = nullptr;
  const int mapCode = p->m_pCurMap->GetMapCode();
  CItemStoreManager *manager = CItemStoreManager::Instance();
  storeList = manager->GetMapItemStoreListBySerial(mapCode);
  if (!storeList)
  {
    return nullptr;
  }

  for (int j = 0; j < storeList->m_nItemStoreNum; ++j)
  {
    CItemStore *store = &storeList->m_ItemStore[j];
    if ((nStoreCode == -1
        || (store->m_pRec && store->m_pRec->m_nStore_trade == nStoreCode))
      && GetSqrt(store->m_pDum->m_pDumPos->m_fCenterPos, p->m_fCurPos) < 100.0)
    {
      return store;
    }
  }
  return nullptr;
}

int IsStorageCodeWithItemKind(int nTableCode, int nStorageCode)
{
  if (nStorageCode == 1)
  {
    if (nTableCode >= 8)
    {
      return 0;
    }
  }
  else if (nStorageCode == 2)
  {
    if (nTableCode != 8 && nTableCode != 9 && nTableCode != 10)
    {
      return 0;
    }
  }
  else if (nStorageCode == 3)
  {
    if (nTableCode != 15)
    {
      return 0;
    }
  }
  else if (nStorageCode == 4 && nTableCode != 24)
  {
    return 0;
  }
  return 1;
}

char *GetItemEquipCivil(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 7:
    {
      _CloakItem_fld *record = static_cast<_CloakItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 12:
    {
      _BagItem_fld *record = static_cast<_BagItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 14:
    {
      _FaceItem_fld *record = static_cast<_FaceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 15:
    {
      _ForceItem_fld *record = static_cast<_ForceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 19:
    {
      _UnitKeyItem_fld *record = static_cast<_UnitKeyItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 24:
    {
      _AnimusItem_fld *record = static_cast<_AnimusItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 32:
    {
      _FIRECRACKER_fld *record = static_cast<_FIRECRACKER_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 35:
    {
      _NPCLink_fld *record = static_cast<_NPCLink_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strCivil : szDefCivilCode;
    }
    default:
      return szDefCivilCode;
  }
}

bool IsItemEquipCivil(int nTableCode, int nItemIndex, unsigned __int8 byRaceSex)
{
  if (byRaceSex > 5u)
  {
    return false;
  }
  char *civil = GetItemEquipCivil(nTableCode, nItemIndex);
  return !civil || civil[byRaceSex] == '1';
}

int IsAbrItem(int nTableCode, int nItemIndex)
{
switch (nTableCode)
  {
    case 6:
      return 0;
    case 10:
    case 11:
    case 16:
    case 19:
    case 25:
    case 27:
      return 1;
    default:
      return 0;
  }
}

int IsSellItem(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 7:
    {
      _CloakItem_fld *record = static_cast<_CloakItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 12:
    {
      _BagItem_fld *record = static_cast<_BagItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 15:
    {
      _ForceItem_fld *record = static_cast<_ForceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 17:
    {
      _OreItem_fld *record = static_cast<_OreItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 19:
    {
      _UnitKeyItem_fld *record = static_cast<_UnitKeyItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 24:
    {
      _AnimusItem_fld *record = static_cast<_AnimusItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 32:
    {
      _FIRECRACKER_fld *record = static_cast<_FIRECRACKER_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 35:
    {
      _NPCLink_fld *record = static_cast<_NPCLink_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bSell) : 0;
    }
    default:
      return 0;
  }
}

void MakeBinaryStr(const unsigned __int8 *pBuff, unsigned __int64 tBufSize, char *pOut, rsize_t tOutSize)
{
  char temp[32]{};

  sprintf_s(temp, sizeof(temp), "0x%0.2X", pBuff[0]);
  strcat_s(pOut, tOutSize, temp);
  for (unsigned __int64 i = 1; i < tBufSize; ++i)
  {
    sprintf_s(temp, sizeof(temp), "%0.2X", pBuff[i]);
    strcat_s(pOut, tOutSize, temp);
  }
}

unsigned __int64 SplitString(char *strSrc, const char *_Delim, std::vector<std::string> *stringlist)
{
  for (char *token = strtok(strSrc, _Delim); token; token = strtok(nullptr, _Delim))
  {
    stringlist->push_back(std::string(token));
  }
  return stringlist->size();
}

CMerchant *FindEmptyNPC(CMerchant *pNPC, int nMax)
{
    unsigned int dwTime = timeGetTime();
    for (int i = 0; i < nMax; ++i)
    {
        if (!pNPC[i].m_bLive)
        {
            if (dwTime - pNPC[i].m_dwLastDestroyTime > 30000)
                return &pNPC[i];
        }
    }
    return nullptr;
}

void NetTrace(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    char Buffer[272]{};
    vsprintf_s(Buffer, sizeof(Buffer), fmt, va);
    va_end(va);
    strcat_s(Buffer, sizeof(Buffer), "\n");
    OutputDebugStringA(Buffer);
}

void __trace(const char *fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  char buffer[272]{};
  vsprintf_s(buffer, sizeof(buffer), fmt, va);
  va_end(va);
  strcat_s(buffer, sizeof(buffer), "\n");
  OutputDebugStringA(buffer);
}

static unsigned __int64 dw64Cnt;

void OutputDebugLog(const char *szFormat, ...)
{
  if (g_Main.IsReleaseServiceMode())
  {
    return;
  }

  va_list args;
  va_start(args, szFormat);
  CStringA message;
  message.FormatV(szFormat, args);
  va_end(args);
  message += "\r\n";

  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  const unsigned __int64 logIndex = dw64Cnt++;
  CStringA line;
  line.Format(
    "%u\t%04d-%02d-%02d %02d:%02d:%02d.%03d : %s",
    static_cast<unsigned int>(logIndex),
    systemTime.wYear,
    systemTime.wMonth,
    systemTime.wDay,
    systemTime.wHour,
    systemTime.wMinute,
    systemTime.wSecond,
    systemTime.wMilliseconds,
    static_cast<const char *>(message));
  OutputDebugStringA(line);
}
int GetCurrentDay()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_mday;
}

int GetCurrentYear()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_year + 1900;
}

int GetCurrentMonth()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_mon + 1;
}

void GetTodayStr(char *szToday)
{
  if (!szToday)
  {
    return;
  }

  char month[32]{};
  char day[16]{};
  const unsigned __int16 year = static_cast<unsigned __int16>(GetCurrentYear());
  const unsigned __int16 curMonth = static_cast<unsigned __int16>(GetCurrentMonth());
  const unsigned __int16 curDay = static_cast<unsigned __int16>(GetCurrentDay());

  if (curMonth > 9)
  {
    sprintf_s(month, sizeof(month), "%d", curMonth);
  }
  else
  {
    sprintf_s(month, sizeof(month), "0%d", curMonth);
  }

  if (curDay > 9)
  {
    sprintf_s(day, sizeof(day), "%d", curDay);
  }
  else
  {
    sprintf_s(day, sizeof(day), "0%d", curDay);
  }

  constexpr size_t kDateSize = 9;
  sprintf_s(szToday, kDateSize, "%d%s%s", year, month, day);
}

unsigned int GetLocalDate()
{
  const int year = GetCurrentYear();
  const int month = GetCurrentMonth();
  const int day = GetCurrentDay();
  return static_cast<unsigned int>(10000 * year + 100 * month + day);
}

unsigned int eGetLocalDate()
{
  const int year = GetCurrentYear();
  const int month = GetCurrentMonth();
  const int day = GetCurrentDay();
  return static_cast<unsigned int>(10000 * year + 100 * month + day);
}

int GetCurwDay()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_wday;
}

void GetNowDateTime(char *szDateTime)
{
  char month[32];
  char day[32];
  char hour[32];
  char minute[16];

  const unsigned __int16 year = static_cast<unsigned __int16>(GetCurrentYear());
  const unsigned __int16 curMonth = static_cast<unsigned __int16>(GetCurrentMonth());
  const unsigned __int16 curDay = static_cast<unsigned __int16>(GetCurrentDay());
  const unsigned __int16 curHour = static_cast<unsigned __int16>(GetCurrentHour());
  const unsigned __int16 curMin = static_cast<unsigned __int16>(GetCurrentMin());

  memset(month, 0, 4);
  memset(day, 0, 4);
  memset(hour, 0, 4);
  memset(minute, 0, 4);

  if (curMonth > 9)
    sprintf_s(month, sizeof(month), "%d", curMonth);
  else
    sprintf_s(month, sizeof(month), "0%d", curMonth);

  if (curDay > 9)
    sprintf_s(day, sizeof(day), "%d", curDay);
  else
    sprintf_s(day, sizeof(day), "0%d", curDay);

  if (curHour > 9)
    sprintf_s(hour, sizeof(hour), "%d", curHour);
  else
    sprintf_s(hour, sizeof(hour), "0%d", curHour);

  if (curMin > 9)
    sprintf_s(minute, sizeof(minute), "%d", curMin);
  else
    sprintf_s(minute, sizeof(minute), "0%d", curMin);

  constexpr size_t kDateTimeSize = 17;
  sprintf_s(szDateTime, kDateTimeSize, "%d-%s-%s %s:%s", year, month, day, hour, minute);
}

unsigned __int8 GetItemKindCode(int nTableCode)
{
  if (nTableCode == 19)
  {
    return 2;
  }
  return nTableCode == 24;
}

void GetSubDayStr(int nSubDay, char *szOutDay)
{
  if (!szOutDay)
  {
    return;
  }

  char monthBuf[32]{};
  char dayBuf[36]{};

  ATL::CTime now = ATL::CTime::GetTickCount();
  ATL::CTimeSpan span(nSubDay, 0, 0, 0);
  ATL::CTime target = now - span;

  const unsigned __int16 year = static_cast<unsigned __int16>(target.GetYear());
  const unsigned __int16 month = static_cast<unsigned __int16>(target.GetMonth());
  const unsigned __int16 day = static_cast<unsigned __int16>(target.GetDay());

  if (month > 9u)
  {
    sprintf_s(monthBuf, sizeof(monthBuf), "%d", month);
  }
  else
  {
    sprintf_s(monthBuf, sizeof(monthBuf), "0%d", month);
  }

  if (day > 9u)
  {
    sprintf_s(dayBuf, sizeof(dayBuf), "%d", day);
  }
  else
  {
    sprintf_s(dayBuf, sizeof(dayBuf), "0%d", day);
  }

  constexpr size_t kDateSize = 9;
  sprintf_s(szOutDay, kDateSize, "%d%s%s", year, monthBuf, dayBuf);
}

char *DisplayItemUpgInfo(int nTableCode, int dwLvBit)
{
  static char g_szLv[32] = "00000000";
  static char g_szUPT[32] = "f";
  static char szBufUpt[32];

  if (!dwLvBit)
  {
    return g_szUPT;
  }

  if (GetItemKindCode(nTableCode) || dwLvBit == 268435455)
  {
    return g_szUPT;
  }

  strcpy_s(g_szLv, "00000000");
  _itoa_s(dwLvBit, szBufUpt, 16);
  const size_t len = std::strlen(szBufUpt);
  std::memcpy(&g_szLv[8 - len], szBufUpt, len);
  g_szLv[8] = '\0';
  if (!std::strcmp(&g_szLv[1], "fffffff"))
  {
    g_szLv[2] = '\0';
  }
  return g_szLv;
}

void IOFileWrite_0(char *pszFileName, unsigned int nLen, char *pszData)
{
  HANDLE hFile = CreateFileA(pszFileName, 0x40000000u, 1u, nullptr, 4u, 0x80u, nullptr);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    SetFilePointer(hFile, 0, nullptr, 2u);
    DWORD written = 0;
    WriteFile(hFile, pszData, nLen, &written, nullptr);
    CloseHandle(hFile);
  }
}

void IOFileWrite_1(char *pszFileName, unsigned int nLen, char *pszData)
{
  HANDLE hFile = CreateFileA(pszFileName, 0x40000000u, 1u, nullptr, 4u, 0x80u, nullptr);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    SetFilePointer(hFile, 0, nullptr, 2u);
    DWORD written = 0;
    WriteFile(hFile, pszData, nLen, &written, nullptr);
    CloseHandle(hFile);
  }
}

void IOFileWrite_2(char *pszFileName, unsigned int nLen, char *pszData)
{
  HANDLE hFile = CreateFileA(pszFileName, 0x40000000u, 1u, nullptr, 4u, 0x80u, nullptr);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    SetFilePointer(hFile, 0, nullptr, 2u);
    DWORD written = 0;
    WriteFile(hFile, pszData, nLen, &written, nullptr);
    CloseHandle(hFile);
  }
}

void IOFileWrite(char *pszFileName, unsigned int nLen, char *pszData)
{
  HANDLE hFile = CreateFileA(pszFileName, 0x40000000u, 1u, nullptr, 4u, 0x80u, nullptr);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    SetFilePointer(hFile, 0, nullptr, 2u);
    DWORD written = 0;
    WriteFile(hFile, pszData, nLen, &written, nullptr);
    CloseHandle(hFile);
  }
}

__time64_t time_20(__int64 *_Time)
{
  return _time64(_Time);
}

__time64_t time_18(__int64 *_Time)
{
  return _time64(_Time);
}

__time64_t time_17(__int64 *_Time)
{
  return _time64(_Time);
}

__time64_t time_5(__int64 *_Time)
{
  return _time64(_Time);
}

__time64_t mktime_3(tm *_Tm)
{
  return _mktime64(_Tm);
}

tm *localtime_5(const __int64 *_Time)
{
  return _localtime64(_Time);
}

tm *localtime_2(const __int64 *_Time)
{
  return _localtime64(_Time);
}

tm *localtime_3(const __int64 *_Time)
{
  return _localtime64(_Time);
}

int GetCurDay()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_mday;
}

int GetNextDay()
{
  std::time_t now = std::time(nullptr);
  now += 86400;
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_mday;
}

bool GetDateStrAfterDay(char *szDate, int iBuffSize, unsigned __int16 wDayAfter)
{
  if (!szDate)
  {
    return false;
  }

  std::time_t now = std::time(nullptr);
  now += static_cast<std::time_t>(86400) * static_cast<std::time_t>(wDayAfter);

  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return false;
  }

  sprintf_s(szDate, iBuffSize, "%04d%02d%02d", local.tm_year + 1900, local.tm_mon + 1, local.tm_mday);
  return true;
}

bool IsDayChanged(int *iOldDay)
{
  const int curDay = GetCurDay();
  if (curDay < 0)
  {
    return false;
  }
  if (curDay == *iOldDay)
  {
    return false;
  }
  *iOldDay = curDay;
  return true;
}

bool GetDateTimeStr(char *szTime)
{
  if (szTime == nullptr)
  {
    return false;
  }

  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return false;
  }

  return std::strftime(szTime, 128, "%y-%m-%d_%H-%M-%S", &local) != 0;
}

bool GetLastWriteFileTime(const char *szFileName, _FILETIME *ftWrite)
{
  if (!szFileName || !ftWrite)
  {
    return false;
  }

  HANDLE hFile = CreateFileA(szFileName, 0x80000000, 1u, nullptr, 3u, 0x80u, nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  _FILETIME creationTime{};
  _FILETIME lastAccessTime{};
  const BOOL ok = GetFileTime(hFile, &creationTime, &lastAccessTime, ftWrite);
  CloseHandle(hFile);
  return ok != FALSE;
}

void FloatToShort(float *pFloat, short *pShort, int size)
{
  for (int j = 0; j < size; ++j)
  {
    pShort[j] = static_cast<short>(static_cast<int>(pFloat[j]));
  }
}

void ShortToFloat(__int16 *pShort, float *pFloat, int size)
{
  for (int j = 0; j < size; ++j)
  {
    pFloat[j] = static_cast<float>(pShort[j]);
  }
}

int GetItemTableCode(const char *psItemCode)
{
  if (psItemCode == nullptr)
  {
    return -1;
  }

  char prefix[3] = {};
  strncpy_s(prefix, sizeof(prefix), psItemCode, 2);

  if (strcmp(prefix, "iu") == 0) return 0;
  if (strcmp(prefix, "il") == 0) return 1;
  if (strcmp(prefix, "ig") == 0) return 2;
  if (strcmp(prefix, "is") == 0) return 3;
  if (strcmp(prefix, "ih") == 0) return 4;
  if (strcmp(prefix, "id") == 0) return 5;
  if (strcmp(prefix, "iw") == 0) return 6;
  if (strcmp(prefix, "im") == 0) return 11;
  if (strcmp(prefix, "ie") == 0) return 12;
  if (strcmp(prefix, "ip") == 0) return 13;
  if (strcmp(prefix, "ib") == 0) return 10;
  if (strcmp(prefix, "if") == 0) return 14;
  if (strcmp(prefix, "ic") == 0) return 15;
  if (strcmp(prefix, "it") == 0) return 16;
  if (strcmp(prefix, "io") == 0) return 17;
  if (strcmp(prefix, "ir") == 0) return 18;
  if (strcmp(prefix, "in") == 0) return 19;
  if (strcmp(prefix, "iy") == 0) return 20;
  if (strcmp(prefix, "ik") == 0) return 7;
  if (strcmp(prefix, "ii") == 0) return 8;
  if (strcmp(prefix, "ia") == 0) return 9;
  if (strcmp(prefix, "iz") == 0) return 21;
  if (strcmp(prefix, "iq") == 0) return 22;
  if (strcmp(prefix, "ix") == 0) return 23;
  if (strcmp(prefix, "ij") == 0) return 24;
  if (strcmp(prefix, "gt") == 0) return 25;
  if (strcmp(prefix, "tr") == 0) return 26;
  if (strcmp(prefix, "sk") == 0) return 27;
  if (strcmp(prefix, "ti") == 0) return 28;
  if (strcmp(prefix, "ev") == 0) return 29;
  if (strcmp(prefix, "re") == 0) return 30;
  if (strcmp(prefix, "bx") == 0) return 31;
  if (strcmp(prefix, "fi") == 0) return 32;
  if (strcmp(prefix, "un") == 0) return 33;
  if (strcmp(prefix, "rd") == 0) return 34;
  if (strcmp(prefix, "lk") == 0) return 35;
  if (strcmp(prefix, "cu") == 0) return 36;

  return -1;
}

int IsOverLapItem(int nTableCode)
{
  return nTableCode == 13
      || nTableCode == 17
      || nTableCode == 18
      || nTableCode == 20
      || nTableCode == 22
      || nTableCode == 31
      || nTableCode == 23
      || nTableCode == 30
      || nTableCode == 26
      || nTableCode == 32
      || nTableCode == 34
      || nTableCode == 35;
}

int IsItemSerialNum(int nTableCode)
{
  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    case 6:
    case 8:
    case 9:
    case 15:
    case 24:
    case 33:
      return 1;
    default:
      return 0;
  }
}

int IsStorageRange(unsigned __int8 byStorageCode, unsigned __int8 byStorageIndex)
{
  switch (byStorageCode)
  {
    case 0u:
      return byStorageIndex < 100;
    case 1u:
      return byStorageIndex < 8u;
    case 2u:
      return byStorageIndex < 7u;
    case 3u:
      return byStorageIndex < 88;
    case 4u:
      return byStorageIndex < 4u;
    case 5u:
      return byStorageIndex < 100;
    case 6u:
      return byStorageIndex < 40;
    case 7u:
      return byStorageIndex < 40;
    default:
      return 0;
  }
}

unsigned __int8 GetItemUpgedLv(unsigned int dwLvBit)
{
  if (!dwLvBit)
  {
    return 0;
  }

  unsigned __int8 level = 0;
  for (int j = 0; j < 7; ++j)
  {
    const unsigned __int8 talik = static_cast<unsigned __int8>((dwLvBit >> (4 * j)) & 0xF);
    if (talik == 15)
    {
      break;
    }
    ++level;
  }
  return level;
}

unsigned __int8 GetItemUpgLimSocket(unsigned int dwLvBit)
{
  if (dwLvBit)
  {
    return static_cast<unsigned __int8>(dwLvBit >> 28);
  }
  return 0;
}

unsigned __int8 GetTalikFromSocket(unsigned int dwLvBit, unsigned __int8 bySocketIndex)
{
  if (dwLvBit)
  {
    return static_cast<unsigned __int8>((dwLvBit >> (4 * bySocketIndex)) & 0xF);
  }
  return 15;
}

unsigned __int8 GetDefItemUpgSocketNum(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  unsigned __int8 socketNum = 0;
  if (nTableCode >= 0)
  {
    if (nTableCode <= 5 || nTableCode == 7)
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        socketNum = static_cast<unsigned __int8>(record->m_nUpgrade);
      }
    }
    else if (nTableCode == 6)
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        socketNum = static_cast<unsigned __int8>(record->m_nUpgrade);
      }
    }
  }

  if (socketNum > 7)
  {
    char buffer[144];
    sprintf_s(buffer, sizeof(buffer), "tbl:%d, idx:%d => slot: %d\n", nTableCode, nItemIndex, socketNum);
    OutputDebugStringA(buffer);
    return 0;
  }
  return socketNum;
}

unsigned int GetBitAfterSetLimSocket(unsigned __int8 byLimSocketNum)
{
  return (static_cast<unsigned int>(byLimSocketNum) << 28) | 0x0FFFFFFF;
}

unsigned int GetBitAfterUpgrade(unsigned int dwCurBit, char dwTalikCode, unsigned __int8 byCurLv)
{
  const unsigned int talikBits = static_cast<unsigned int>(dwTalikCode & 0xF);
  const unsigned int shift = 4u * byCurLv;
  const unsigned int leftMask = ~(-1 << shift);
  const unsigned int upperBits = (dwCurBit >> (shift + 4)) << (shift + 4);
  return (talikBits << shift) | (dwCurBit & leftMask) | upperBits;
}

unsigned int GetBitAfterDowngrade(unsigned int dwCurBit, unsigned __int8 byCurLv)
{
  return (15u << (4u * (byCurLv - 1))) | dwCurBit;
}

_animus_fld *GetAnimusFldFromExp(int nAnimusClass, unsigned __int64 dwExp)
{

  CRecordData *table = &CAnimus::s_tblParameter[nAnimusClass];
  for (int n = 0; n < 65; ++n)
  {
    _animus_fld *record = reinterpret_cast<_animus_fld *>(table->GetRecord(n));
    if (!record)
      return nullptr;
    if (record->m_nForLvUpExp > dwExp)
      return record;
  }
  const int recordNum = static_cast<int>(table->GetRecordNum());
  return reinterpret_cast<_animus_fld *>(table->GetRecord(recordNum - 1));
}

unsigned int GetMaxParamFromExp(int nAnimusClass, unsigned __int64 dwExp)
{

  _animus_fld *record = GetAnimusFldFromExp(nAnimusClass, dwExp);
  if (!record)
    return 0;
  const unsigned int maxHP = static_cast<unsigned int>(record->m_nMaxHP) & 0xFFFFu;
  const unsigned int maxFP = static_cast<unsigned int>(record->m_nMaxFP) & 0xFFFFu;
  return (maxFP << 16) | maxHP;
}

int GetMaxResKind()
{
  return static_cast<int>(g_Main.m_tblItemData[18].GetRecordNum());
}

unsigned __int8 GetItemGrade(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  if (nTableCode >= 0)
  {
    if (nTableCode <= 5 || nTableCode == 7)
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        return static_cast<unsigned __int8>(record->m_nItemGrade);
      }
    }
    else if (nTableCode == 6)
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        return static_cast<unsigned __int8>(record->m_nItemGrade);
      }
    }
  }
  return 0;
}

int GetItemEquipLevel(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nLevelLim;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nLevelLim;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nLevelLim;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nLevelLim;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nLevelLim;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nLevelLim;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nLevelLim;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nLevelLim;
    }
    default:
      return 1;
  }
}

int GetItemEquipUpLevel(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nUpLevelLim;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nUpLevelLim;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nUpLevelLim;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nUpLevelLim;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nUpLevelLim;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nUpLevelLim;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nUpLevelLim;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 1;
      return record->m_nUpLevelLim;
    }
    default:
      return 1;
  }
}

_EQUIP_MASTERY_LIM *GetItemEquipMastery(int nTableCode, int nItemIndex, int *pnLimNum)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  if (nTableCode >= 0)
  {
    if (nTableCode <= 5 || nTableCode == 7)
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pnLimNum = 2;
        return record->m_MasteryLimit;
      }
    }
    else if (nTableCode == 6)
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pnLimNum = 2;
        return record->m_MasteryLimit;
      }
    }
  }
  return nullptr;
}

unsigned __int8 GetWeaponClass(int nItemIndex)
{
  _WeaponItem_fld *record = reinterpret_cast<_WeaponItem_fld *>(s_ptblItemData[6].GetRecord(nItemIndex));
  if (!record)
  {
    return 0;
  }

  const int weaponType = record->m_nType;
  switch (weaponType)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 9:
      return 0;
    case 5:
    case 6:
    case 7:
    case 8:
    case 11:
      return 1;
    default:
      return 0;
  }
}

int CalcRoundUp(float fVal)
{
  int rounded = static_cast<int>(fVal);
  const float remainder = fVal - static_cast<float>(rounded);
  if (remainder > 0.0f)
  {
    ++rounded;
  }
  return rounded;
}

int CalcMastery(int nMasteryCode, int nMasteryIndex, int dwMasteryCum, unsigned int nRaceCode)
{
  float mastery = 0.0f;
  switch (nMasteryCode)
  {
    case 0:
    {
      const float baseTerm = (static_cast<float>(dwMasteryCum) + 1.0f) / 1000.0f;
      const float sqrtTerm = std::sqrt(static_cast<float>(dwMasteryCum) + 1.0f);
      mastery = std::sqrt(baseTerm + sqrtTerm);
      break;
    }
    case 1:
    {
      const float baseTerm = (static_cast<float>(dwMasteryCum) + 1.0f) / 1000.0f;
      const float sqrtTerm = std::sqrt(static_cast<float>(dwMasteryCum) + 1.0f);
      mastery = std::sqrt(baseTerm + sqrtTerm);
      break;
    }
    case 2:
    {
      const float baseTerm = (static_cast<float>(dwMasteryCum) + 1.0f) / 100.0f;
      const float sqrtTerm = std::sqrt(static_cast<float>(dwMasteryCum) + 1.0f);
      mastery = std::sqrt(baseTerm + sqrtTerm);
      break;
    }
    case 3:
    {
      const float scaledRoot = std::sqrt((static_cast<float>(dwMasteryCum) + 1.0f) * 10.0f);
      mastery = std::sqrt(scaledRoot);
      break;
    }
    case 4:
    {
      const float scaledRoot = std::sqrt((static_cast<float>(dwMasteryCum) + 1.0f) * 14.0f);
      mastery = std::sqrt(scaledRoot);
      break;
    }
    case 5:
    {
      if (nMasteryIndex == 0 || nMasteryIndex == 1)
      {
        mastery = std::sqrt(((static_cast<float>(dwMasteryCum) / 1.1f) * 3.0f) + 1.0f);
      }
      else if (nMasteryIndex == 2)
      {
        mastery = std::sqrt(((static_cast<float>(dwMasteryCum) / 10.0f) * 3.0f) + 1.0f);
      }
      break;
    }
    case 6:
    {
      if (nRaceCode > 1)
      {
        const float baseTerm = (static_cast<float>(dwMasteryCum) + 1.0f) / 1000.0f;
        const float sqrtTerm = std::sqrt(static_cast<float>(dwMasteryCum) + 1.0f);
        mastery = std::sqrt(baseTerm + sqrtTerm);
      }
      else
      {
        mastery = std::sqrt(static_cast<float>(dwMasteryCum) / 15000.0f) + 1.0f;
      }
      break;
    }
    default:
      break;
  }

  if (mastery <= 99.0f)
  {
    if (mastery < 1.0f)
    {
      mastery = FLOAT_1_0;
    }
  }
  else
  {
    mastery = 99.0f;
  }
  return static_cast<int>(mastery);
}

int GetSFLevel(int nLv, unsigned int dwHitCount)
{
  const float ratio =
    std::sqrt(static_cast<float>(static_cast<int>(dwHitCount + 1)) / sR[nLv]);
  const int level = static_cast<int>(std::sqrt(ratio) + 0.9999f);
  if (level <= 7)
  {
    if (level < 1)
    {
      return 1;
    }
  }
  else
  {
    return 7;
  }
  return level;
}

int GetStaffMastery(unsigned int *pdwForceLvCum)
{
  const float weightedSum = (static_cast<float>(static_cast<int>(pdwForceLvCum[0])) * 1.125f)
                            + (static_cast<float>(static_cast<int>(pdwForceLvCum[1])) * 2.25f)
                            + (static_cast<float>(static_cast<int>(pdwForceLvCum[2])) * 3.375f)
                            + (static_cast<float>(static_cast<int>(pdwForceLvCum[3])) * 4.5f);
  const float baseTerm = (weightedSum + 1.0f) / 1000.0f;
  const float sqrtTerm = std::sqrt(weightedSum + 1.0f);
  const int mastery = static_cast<int>(std::sqrt(baseTerm + sqrtTerm));
  if (mastery <= 99)
  {
    if (mastery < 1)
    {
      return 1;
    }
  }
  else
  {
    return 99;
  }
  return mastery;
}

void InitMasteryFormula(CRecordData *pSkillData, CRecordData *pForceData)
{
  std::memset(s_nSkillLvPerMastery, -1, sizeof(s_nSkillLvPerMastery));
  std::memset(s_nForceLvPerMastery, -1, sizeof(s_nForceLvPerMastery));
  for (int n = 0;; ++n)
  {
    const int recordNum = pSkillData->GetRecordNum();
    if (n >= recordNum)
    {
      break;
    }
    _skill_fld *record = static_cast<_skill_fld *>(pSkillData->GetRecord(n));
    if (record && static_cast<unsigned int>(record->m_nMastIndex) < 8u)
    {
      s_nSkillLvPerMastery[record->m_nMastIndex] = record->m_nLv;
    }
  }
  for (int n = 0;; ++n)
  {
    const int recordNum = pForceData->GetRecordNum();
    if (n >= recordNum)
    {
      break;
    }
    _force_fld *record = static_cast<_force_fld *>(pForceData->GetRecord(n));
    if (record && static_cast<unsigned int>(record->m_nMastIndex) < 24)
    {
      s_nForceLvPerMastery[record->m_nMastIndex] = record->m_nLv;
    }
  }
}

int GetItemStdPrice(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind)
{
  (void)nRace;
  CRecordData *table = &s_ptblItemData[nTableCode];

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 12:
    {
      _BagItem_fld *record = static_cast<_BagItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 15:
    {
      _ForceItem_fld *record = static_cast<_ForceItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 17:
    {
      _OreItem_fld *record = static_cast<_OreItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nAncStdPrice;
    }
    case 19:
      return 0;
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 24:
    {
      _AnimusItem_fld *record = static_cast<_AnimusItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_bMoney);
      return record->m_nStdPrice;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 32:
    {
      _FIRECRACKER_fld *record = static_cast<_FIRECRACKER_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 35:
    {
      _NPCLink_fld *record = static_cast<_NPCLink_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPrice;
    }
    default:
      return 0;
  }
}

int GetItemStoragePrice(int nTableCode, int nItemIndex, int nRace)
{
  CRecordData *table = &s_ptblItemData[nTableCode];

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 7:
    {
      _CloakItem_fld *record = static_cast<_CloakItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 12:
      return 0;
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 15:
    {
      _ForceItem_fld *record = static_cast<_ForceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 17:
    {
      _OreItem_fld *record = static_cast<_OreItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 19:
      return 0;
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 24:
    {
      _AnimusItem_fld *record = static_cast<_AnimusItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 32:
    {
      _FIRECRACKER_fld *record = static_cast<_FIRECRACKER_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 35:
    {
      _NPCLink_fld *record = static_cast<_NPCLink_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_nStoragePrice) : 0;
    }
    default:
      return 0;
  }
}

bool IsTrunkIOAble(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 7:
    {
      _CloakItem_fld *record = static_cast<_CloakItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 12:
      return false;
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 15:
    {
      _ForceItem_fld *record = static_cast<_ForceItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 17:
    {
      _OreItem_fld *record = static_cast<_OreItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 19:
      return false;
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 24:
    {
      _AnimusItem_fld *record = static_cast<_AnimusItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 32:
    {
      _FIRECRACKER_fld *record = static_cast<_FIRECRACKER_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 35:
    {
      _NPCLink_fld *record = static_cast<_NPCLink_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(nItemIndex));
      return record && record->m_bStoragePossible != 0;
    }
    default:
      return false;
  }
}

int GetItemStdPoint(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind)
{
  CRecordData *table = &s_ptblItemData[nTableCode];

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 12:
    {
      _BagItem_fld *record = static_cast<_BagItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 15:
    {
      _ForceItem_fld *record = static_cast<_ForceItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 17:
    {
      _OreItem_fld *record = static_cast<_OreItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      switch (nRace)
      {
        case 0:
          return record->m_nAncStdPrice;
        case 1:
          return record->m_nExStdPrice;
        case 2:
          return record->m_nMecaStdPrice;
        default:
          return record->m_nStdPoint;
      }
    }
    case 19:
      return 0;
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 24:
    {
      _AnimusItem_fld *record = static_cast<_AnimusItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_bMoney);
      return record->m_nStdPoint;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 32:
    {
      _FIRECRACKER_fld *record = static_cast<_FIRECRACKER_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
        return 0;
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nStdPoint;
    }
    default:
      return 0;
  }
}

int GetItemProcPoint(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind)
{
  (void)nRace;
  CRecordData *table = &s_ptblItemData[nTableCode];

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nProcPoint;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nProcPoint;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nProcPoint;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nProcPoint;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nProcPoint;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nProcPoint;
    }
    case 12:
    {
      _BagItem_fld *record = static_cast<_BagItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nProcPoint;
    }
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nProcPoint;
    }
    case 15:
    {
      _ForceItem_fld *record = static_cast<_ForceItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nProcPoint;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nProcPoint;
    }
    case 17:
    {
      _OreItem_fld *record = static_cast<_OreItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nProcPoint;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nProcPoint;
    }
    case 19:
      return 0;
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nProcPoint;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nProcPoint;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nProcPoint;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nProcPoint;
    }
    case 24:
    {
      _AnimusItem_fld *record = static_cast<_AnimusItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nProcPoint;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nProcPoint;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nProcPoint;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nProcPoint;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nProcPoint;
    }
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_bMoney);
      return record->m_nProcPoint;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nProcPoint;
    }
    case 32:
    {
      _FIRECRACKER_fld *record = static_cast<_FIRECRACKER_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nProcPoint;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nProcPoint;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nProcPoint;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nProcPoint;
    }
    default:
      return 0;
  }
}

int GetItemKillPoint(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind)
{
  (void)nRace;
  CRecordData *table = &s_ptblItemData[nTableCode];

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nKillPoint;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nKillPoint;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nKillPoint;
    }
    case 12:
    {
      _BagItem_fld *record = static_cast<_BagItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 15:
    {
      _ForceItem_fld *record = static_cast<_ForceItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 17:
    {
      _OreItem_fld *record = static_cast<_OreItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 19:
      return 0;
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 24:
    {
      _AnimusItem_fld *record = static_cast<_AnimusItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_bMoney);
      return record->m_nKillPoint;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 32:
    {
      _FIRECRACKER_fld *record = static_cast<_FIRECRACKER_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nKillPoint;
    }
    default:
      return 0;
  }
}

int GetItemGoldPoint(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind)
{
  (void)nRace;
  CRecordData *table = &s_ptblItemData[nTableCode];

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nGoldPoint;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nGoldPoint;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nItemIndex));
      if (record)
      {
        *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      }
      return record->m_nGoldPoint;
    }
    case 12:
    {
      _BagItem_fld *record = static_cast<_BagItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 15:
    {
      _ForceItem_fld *record = static_cast<_ForceItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 17:
    {
      _OreItem_fld *record = static_cast<_OreItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 19:
      return 0;
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 24:
    {
      _AnimusItem_fld *record = static_cast<_AnimusItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_bMoney);
      return record->m_nGoldPoint;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 32:
    {
      _FIRECRACKER_fld *record = static_cast<_FIRECRACKER_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(nItemIndex));
      if (!record)
      {
        return 0;
      }
      *pbyMoneyKind = static_cast<unsigned __int8>(record->m_nMoney);
      return record->m_nGoldPoint;
    }
    default:
      return 0;
  }
}

int GetUsePcCashType(unsigned __int8 byTblCode, int nIndex)
{
  CRecordData *table = &s_ptblItemData[byTblCode];
  if (!CashItemRemoteStore::FindCashRec(byTblCode, nIndex))
  {
    return 0;
  }

  if (byTblCode == 10)
  {
    _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nIndex));
    if (record)
    {
      return record->m_nUsePCCash;
    }
  }
  else
  {
    switch (byTblCode)
    {
      case 13:
      {
        _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nIndex));
        if (record)
        {
          return record->m_nUsePCCash;
        }
        break;
      }
      case 18:
      {
        _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nIndex));
        if (record)
        {
          return record->m_nUsePCCash;
        }
        break;
      }
      case 22:
      {
        _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nIndex));
        if (record)
        {
          return record->m_nUsePCCash;
        }
        break;
      }
      case 31:
      {
        _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nIndex));
        if (record)
        {
          return record->m_nUsePCCash;
        }
        break;
      }
      default:
        break;
    }
  }
  return 0;
}

int IsCashItem(unsigned __int8 byTblCode, int dwIndex)
{
  CRecordData *table = &s_ptblItemData[byTblCode];
  switch (byTblCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      return 0;
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(dwIndex));
      if (!record)
        return 0;
      return static_cast<unsigned int>(record->m_bIsCash);
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(dwIndex));
      if (!record)
        return 0;
      return static_cast<unsigned int>(record->m_bIsCash);
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(dwIndex));
      if (!record)
        return 0;
      return static_cast<unsigned int>(record->m_bIsCash);
    }
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(dwIndex));
      if (!record)
        return 0;
      return static_cast<unsigned int>(record->m_bIsCash);
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(dwIndex));
      if (!record)
        return 0;
      return static_cast<unsigned int>(record->m_bIsCash);
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(dwIndex));
      if (!record)
        return 0;
      return static_cast<unsigned int>(record->m_bIsCash);
    }
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(dwIndex));
      if (!record)
        return 0;
      return static_cast<unsigned int>(record->m_bIsCash);
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(dwIndex));
      if (!record)
        return 0;
      return static_cast<unsigned int>(record->m_bIsCash);
    }
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(dwIndex));
      if (!record)
        return 0;
      return static_cast<unsigned int>(record->m_bIsCash);
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(dwIndex));
      if (!record)
        return 0;
      return static_cast<unsigned int>(record->m_bIsCash);
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(dwIndex));
      if (!record)
        return 0;
      return static_cast<unsigned int>(record->m_bIsCash);
    }
    case 35:
    {
      _NPCLink_fld *record = static_cast<_NPCLink_fld *>(table->GetRecord(dwIndex));
      if (!record)
        return 0;
      return static_cast<unsigned int>(record->m_bIsCash);
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(dwIndex));
      if (!record)
        return 0;
      return static_cast<unsigned int>(record->m_bIsCash);
    }
    default:
      return 0;
  }
}

unsigned int GetItemDurPoint(int nTableCode, int nIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nIndex));
      if (!record)
        return 1;
      return static_cast<unsigned int>(record->m_nDurUnit);
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nIndex));
      if (!record)
        return 1;
      return static_cast<unsigned int>(record->m_nDurUnit);
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nIndex));
      if (!record)
        return 1;
      return static_cast<unsigned int>(record->m_nDurUnit);
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nIndex));
      if (!record)
        return 1;
      return static_cast<unsigned int>(record->m_nDurUnit);
    }
    case 15:
      return 0;
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nIndex));
      if (!record)
        return 1;
      return static_cast<unsigned int>(record->m_nDurUnit);
    }
    case 24:
      return 0;
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nIndex));
      if (!record)
        return 1;
      return static_cast<unsigned int>(record->m_nMaxHP);
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nIndex));
      if (!record)
        return 1;
      return static_cast<unsigned int>(static_cast<int>(record->m_fMaxHP));
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nIndex));
      if (!record)
        return 1;
      return static_cast<unsigned int>(record->m_nDurUnit);
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nIndex));
      if (!record)
        return 1;
      return static_cast<unsigned int>(record->m_nMaxHP);
    }
    default:
      return 1;
  }
}

char *GetItemKorName(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 7:
    {
      _CloakItem_fld *record = static_cast<_CloakItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 12:
    {
      _BagItem_fld *record = static_cast<_BagItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 14:
    {
      _FaceItem_fld *record = static_cast<_FaceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 15:
    {
      _ForceItem_fld *record = static_cast<_ForceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 17:
    {
      _OreItem_fld *record = static_cast<_OreItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 19:
    {
      _UnitKeyItem_fld *record = static_cast<_UnitKeyItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 24:
    {
      _AnimusItem_fld *record = static_cast<_AnimusItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 29:
    {
      _EventItem_fld *record = static_cast<_EventItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 32:
    {
      _FIRECRACKER_fld *record = static_cast<_FIRECRACKER_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 35:
    {
      _NPCLink_fld *record = static_cast<_NPCLink_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? record->m_strName : szDefItemName;
    }
    default:
      return szDefItemName;
  }
}

int IsTimeItem(unsigned __int8 byTblCode, int dwIndex)
{
  CRecordData *table = &s_ptblItemData[byTblCode];

  switch (byTblCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 7:
    {
      _CloakItem_fld *record = static_cast<_CloakItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 12:
    {
      _BagItem_fld *record = static_cast<_BagItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 17:
    {
      _OreItem_fld *record = static_cast<_OreItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(dwIndex));
      return record ? static_cast<unsigned int>(record->m_bIsTime) : 0;
    }
    default:
      return 0;
  }
}

int ParsingCommandA(char *pszSrc, int nMaxWordNum, char **ppszDst, int nMaxWordSize)
{
  char *src = pszSrc;
  for (int j = 0; j < nMaxWordNum; ++j)
  {
    char *dst = ppszDst[j];
    int len = 0;

    while (*src == ' ' || *src == '\t')
    {
      ++src;
    }

    while (*src != '\0' && *src != ' ' && *src != '\t')
    {
      *dst++ = *src++;
      if (len++ >= nMaxWordSize - 1)
      {
        return 0;
      }
    }
    *dst = '\0';

    if (*src == '\0')
    {
      if (std::strlen(ppszDst[j]) != 0)
      {
        return j + 1;
      }
      return j;
    }

    if (dst == ppszDst[j])
    {
      return j;
    }
    ++src;
  }

  return 0;
}

int ParsingCommandW(char *pwszSrc, int nMaxWordNum, char **ppwszDst, int nMaxWordSize)
{
  char *src = pwszSrc;
  for (int j = 0; ; ++j)
  {
    if (j >= nMaxWordNum)
    {
      return 0;
    }

    char *dst = ppwszDst[j];
    int len = 0;

    while (*src == ' ' || *src == '\t')
    {
      ++src;
    }

    while (*src != '\0' && *src != ' ' && *src != '\t')
    {
      *dst++ = *src++;
      if (len++ >= nMaxWordSize - 1)
      {
        return 0;
      }
    }
    *dst = '\0';

    if (*src == '\0')
    {
      return std::strlen(ppwszDst[j]) ? (j + 1) : j;
    }

    if (dst == ppwszDst[j])
    {
      return j;
    }

    ++src;
  }
}

bool IsSQLValidString(const char *wszStr)
{
  if (!wszStr)
  {
    return true;
  }

  if (strchr(wszStr, ';'))
  {
    return false;
  }
  if (strchr(wszStr, '\''))
  {
    return false;
  }
  if (strstr(wszStr, "--"))
  {
    return false;
  }
  if (strstr(wszStr, "/*"))
  {
    return false;
  }
  if (strstr(wszStr, "*/"))
  {
    return false;
  }
  return strstr(wszStr, "xp_") == nullptr;
}

bool ReadOptionAndWriteDefault(const char *iniPath, const char *section, const char *key, const char *defaultValue, char *output, unsigned __int64 outputSize)
{
  if (!iniPath || !section || !key || !defaultValue || !output || outputSize == 0)
  {
    return false;
  }

  output[0] = '\0';
  GetPrivateProfileStringA(section, key, "", output, static_cast<DWORD>(outputSize), iniPath);
  if (output[0] != '\0')
  {
    return true;
  }

  strcpy_s(output, outputSize, defaultValue);
  WritePrivateProfileStringA(section, key, defaultValue, iniPath);
  return true;
}

void WriteServerStartHistory(const char *format, ...)
{
  char buffer[512]{};
  va_list args;
  va_start(args, format);
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
  va_end(args);
  OutputDebugStringA(buffer);
  OutputDebugStringA("\n");
}

int GetCurrentHour()
{
    return CTime::GetCurrentTime().GetHour();
}

int GetCurrentMin()
{
    return CTime::GetCurrentTime().GetMinute();
}

int GetCurrentSec()
{
    return CTime::GetCurrentTime().GetSecond();
}

void clear_file(const char *directory, int keepCount)
{
  constexpr unsigned int kCutoffScale = 200;

  FILETIME currentFileTime{};
  CoFileTimeNow(&currentFileTime);

  char searchPattern[288]{};
  sprintf_s(searchPattern, sizeof(searchPattern), "%s\\*.*", directory);

  WIN32_FIND_DATAA findData{};
  const HANDLE findHandle = FindFirstFileA(searchPattern, &findData);
  if (findHandle == INVALID_HANDLE_VALUE)
  {
    return;
  }

  char entryPath[272]{};
  while (FindNextFileA(findHandle, &findData))
  {
    if (!std::strcmp(".", findData.cFileName) || !std::strcmp("..", findData.cFileName))
    {
      continue;
    }

    sprintf_s(entryPath, sizeof(entryPath), "%s\\%s", directory, findData.cFileName);
    if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
    {
      clear_file(entryPath, keepCount);
      continue;
    }

    if (findData.ftLastWriteTime.dwHighDateTime < currentFileTime.dwHighDateTime
      && currentFileTime.dwHighDateTime - findData.ftLastWriteTime.dwHighDateTime
        > kCutoffScale * static_cast<unsigned int>(keepCount))
    {
      DeleteFileA(entryPath);
    }
  }

  FindClose(findHandle);
}

int MyMessageBox(const char *title, const char *format, ...)
{
  char buffer[1024]{};
  va_list args;
  va_start(args, format);
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
  va_end(args);
  OutputDebugStringA(buffer);
  OutputDebugStringA("\n");
  MessageBoxA(nullptr, buffer, title, MB_OK | MB_ICONERROR);
  return 0;
}

int MyCrtDebugReportHook(int reportType, char *message, int *returnValue)
{
if (returnValue != nullptr)
  {
    *returnValue = 0;
  }
  return 0;
}

void ServerProgramExit(const char *source, int reason)
{
  char buffer[256]{};
  _snprintf_s(buffer, sizeof(buffer), _TRUNCATE, "ServerProgramExit: %s (%d)", source, reason);
  OutputDebugStringA(buffer);
  OutputDebugStringA("\n");
  ExitProcess(static_cast<unsigned int>(reason));
}

void StripEXT(char *szPath)
{
  int idx = static_cast<int>(std::strlen(szPath)) - 1;
  for (int i = idx; i >= 0; --i)
  {
    if (szPath[i] == '.')
    {
      szPath[i] = '\0';
      return;
    }
  }
}

void StripPath(char *szPath)
{
  const char *lastSlash = std::strrchr(szPath, '\\');
  if (!lastSlash || !lastSlash[1])
  {
    return;
  }

  const size_t keepLen = std::strlen(lastSlash + 1);
  char temp[256]{};
  std::memcpy(temp, lastSlash + 1, static_cast<unsigned int>(keepLen));
  temp[keepLen] = '\0';
  strcpy_s(szPath, 256, temp);
}

void StripName(char *szPath)
{
  int idx = static_cast<int>(std::strlen(szPath)) - 1;
  for (char *cur = &szPath[idx]; *cur != '\\'; --cur)
    --idx;
  if (idx)
    szPath[idx + 1] = '\0';
}

char W2M(char *lpwStr, char *szTran, unsigned int wTranBuffSize)
{
  if (!wTranBuffSize)
  {
    return 0;
  }

  if (std::strlen(lpwStr) + 1 > wTranBuffSize)
  {
    return 0;
  }

  strcpy_s(szTran, wTranBuffSize, lpwStr);
  return 1;
}

char M2W(char *lpStr, char *wszTran, unsigned int wTranBuffSize)
{
  if (!wTranBuffSize)
  {
    return 0;
  }

  if (std::strlen(lpStr) + 1 > wTranBuffSize)
  {
    return 0;
  }

  std::strcpy(wszTran, lpStr);
  return 1;
}

bool IsServerMode()
{
    return dword_184A77F38 != 0;
}

void InitR3Engine(int use)
{
  const int currentMode = static_cast<int>(dword_184A77F38);
  dword_184A77F3C = 1;
  dword_184A77F38 = static_cast<unsigned int>(use == 1 ? 1 : currentMode);

  std::memset(&stState, 0, sizeof(stState));
  stState.mAsfectRatio = 640.0f / 480.0f;
  stState.mNear = 2.0f;
  stState.mFar = 6000.0f;
  stState.mFov = 0.60000002f;
  stState.mMipMapBias = 0.0f;
  stState.mFogColor = 0;
  stState.mFogStart = 0.0f;
  stState.mFogEnd = 0.0f;
  stState.mIsFog = 0;
  stState.mFogRangeEnable = 0;
  stState.mTempSize = 1024u * 256u;
  stState.mGamma = 1.0f;
  stState.mFlySpeed = 30.0f;
  stState.mWideOn = 0;
  stState.mIsDrawLogo = 0;
  stState.mDLightTexture = nullptr;
  stState.mLensFlareTex = -1;
  for (int i = 0; i < 16; ++i)
    stState.mLensFlareScale[i] = 1.0f;

  auto setIdentity = [](float (&matrix)[4][4]) {
    std::memset(matrix, 0, sizeof(matrix));
    matrix[0][0] = 1.0f;
    matrix[1][1] = 1.0f;
    matrix[2][2] = 1.0f;
    matrix[3][3] = 1.0f;
  };
  setIdentity(stState.mMatView);
  setIdentity(stState.mMatWorld);
  setIdentity(stState.mMatProj);

  const char *const iniFile = ".\\R3Engine.ini";
  char valueBuf[256]{};

  auto readStr = [&](const char *section, const char *key, const char *def, char *out, DWORD outSize) {
    GetPrivateProfileStringA(section, key, def, out, outSize, iniFile);
  };

  auto readInt = [&](const char *section, const char *key, const char *def) {
    GetPrivateProfileStringA(section, key, def, valueBuf, static_cast<DWORD>(sizeof(valueBuf)), iniFile);
    return std::atoi(valueBuf);
  };

  auto readFloat = [&](const char *section, const char *key, const char *def) {
    GetPrivateProfileStringA(section, key, def, valueBuf, static_cast<DWORD>(sizeof(valueBuf)), iniFile);
    return static_cast<float>(std::atof(valueBuf));
  };

  auto readBool = [&](const char *section, const char *key, const char *def) {
    GetPrivateProfileStringA(section, key, def, valueBuf, static_cast<DWORD>(sizeof(valueBuf)), iniFile);
    _strupr_s(valueBuf, sizeof(valueBuf));
    return std::strcmp(valueBuf, "TRUE") == 0;
  };

  readStr("Path", "MapPath", ".\\Map\\", stState.MapPath, static_cast<DWORD>(sizeof(stState.MapPath)));
  readStr("Path", "SystemPath", ".\\System\\", stState.SystemPath, static_cast<DWORD>(sizeof(stState.SystemPath)));
  readStr("Path", "EntityPath", ".\\Map\\Entity\\", stState.EntityPath, static_cast<DWORD>(sizeof(stState.EntityPath)));
  readStr("Path", "EffectPath", ".\\Effect\\", stState.EffectPath, static_cast<DWORD>(sizeof(stState.EffectPath)));
  readStr(
    "Path",
    "ScreenShotsPath",
    ".\\ScreenShots\\",
    stState.ScreenShotsPath,
    static_cast<DWORD>(sizeof(stState.ScreenShotsPath)));
  readStr("Path", "SoundPath", ".\\Snd\\", stState.SoundPath, static_cast<DWORD>(sizeof(stState.SoundPath)));

  stState.mScreenXsize = static_cast<float>(readInt("RenderState", "ScreenXSize", "1024"));
  stState.mViewSx = 0;
  stState.mViewXL = static_cast<long>(stState.mScreenXsize);
  stState.mScreenYsize = static_cast<float>(readInt("RenderState", "ScreenYSize", "768"));
  stState.mViewSy = 0;
  stState.mViewYL = static_cast<long>(stState.mScreenYsize);
  stState.mRenderBits = static_cast<unsigned int>(readInt("RenderState", "RenderBits", "32"));
  stState.mZbufferBits = static_cast<unsigned int>(readInt("RenderState", "ZbufferBits", "24"));
  stState.mbFullScreen = readBool("RenderState", "bFullScreen", "FALSE") ? 1 : 0;

  int textureDetail = readInt("RenderState", "TextureDetail", "2");
  if (textureDetail < 0)
    textureDetail = 0;
  if (textureDetail > 3)
    textureDetail = 3;
  stState.mTextureDetail = static_cast<unsigned int>(3 - textureDetail);
  stState.mSceneTextureDetail = stState.mTextureDetail == 0 ? 0 : stState.mTextureDetail - 1;

  int dynamicLight = readInt("RenderState", "DynamicLight", "1");
  if (dynamicLight < 0)
    dynamicLight = 0;
  if (dynamicLight > 3)
    dynamicLight = 3;
  if (dynamicLight == 1)
    stState.mDynamicLight = 3;
  else if (dynamicLight == 2)
    stState.mDynamicLight = 6;
  else if (dynamicLight == 3)
    stState.mDynamicLight = 10;
  else
    stState.mDynamicLight = 0;

  int shadowDetail = readInt("RenderState", "ShadowDetail", "1");
  if (shadowDetail < 0)
    shadowDetail = 0;
  if (shadowDetail > 3)
    shadowDetail = 3;
  if (shadowDetail == 1)
    stState.mShadowDetail = 1;
  else if (shadowDetail == 2)
    stState.mShadowDetail = 10;
  else if (shadowDetail == 3)
    stState.mShadowDetail = static_cast<unsigned int>(-1);
  else
    stState.mShadowDetail = 0;

  stState.mDetailTexture = readBool("RenderState", "bDetailTexture", "TRUE") ? 1 : 0;
  stState.mMouseAccelation = readBool("RenderState", "bMouseAccelation", "TRUE") ? 1 : 0;

  int seeDistanceIndex = readInt("RenderState", "SeeDistance", "2");
  if (seeDistanceIndex < 0)
    seeDistanceIndex = 0;
  if (seeDistanceIndex > 2)
    seeDistanceIndex = 2;
  static const float kSeeDistanceTable[3] = {0.6f, 0.8f, 1.0f};
  stState.mSeeDistance = kSeeDistanceTable[seeDistanceIndex];

  stState.mSoundOn = readBool("Sound", "Sound", "TRUE") ? 1 : 0;
  stState.mMusicOn = readBool("Sound", "Music", "TRUE") ? 1 : 0;
  stState.mMusicVol = readFloat("Sound", "MusicVol", "1.0f");
  stState.mSoundVol = readFloat("Sound", "SoundVol", "1.0f");
  stState.mGamma = readFloat("RenderState", "Gamma", "1.0f");
  stState.mbStartCamera = readBool("RenderState", "bStartCamera", "FALSE") ? 1 : 0;

  readStr("SampleCharacter", "CharPath", ".\\Character\\", stState.CharPath, static_cast<DWORD>(sizeof(stState.CharPath)));
  readStr(
    "SampleCharacter",
    "CharTexturePath",
    ".\\Character\\tex\\",
    stState.CharTexturePath,
    static_cast<DWORD>(sizeof(stState.CharTexturePath)));
  readStr("SampleCharacter", "MeshName", "SNWG_001.msh", stState.MeshName, static_cast<DWORD>(sizeof(stState.MeshName)));
  readStr("SampleCharacter", "BoneName", "bone_b.bn", stState.BoneName, static_cast<DWORD>(sizeof(stState.BoneName)));
  readStr("SampleCharacter", "StopAni", "Stop.ani", stState.StopAniName, static_cast<DWORD>(sizeof(stState.StopAniName)));
  readStr("SampleCharacter", "RunAni", "Run_Loop.ani", stState.RunAniName, static_cast<DWORD>(sizeof(stState.RunAniName)));
  readStr("SampleCharacter", "SwordAni", "", stState.SwordAniName, static_cast<DWORD>(sizeof(stState.SwordAniName)));
  readStr("SampleCharacter", "SwordMesh", "", stState.SwordMeshName, static_cast<DWORD>(sizeof(stState.SwordMeshName)));
  readStr(
    "SampleCharacter",
    "SwordTexturePath",
    "",
    stState.SwordTexturePath,
    static_cast<DWORD>(sizeof(stState.SwordTexturePath)));

  stState.mMainLightNomal[0] = 1.0f;
  stState.mMainLightNomal[1] = 1.0f;
  stState.mMainLightNomal[2] = 1.0f;
  stState.mLightNomalForShadow[0] = 1.0f;
  stState.mLightNomalForShadow[1] = 1.0f;
  stState.mLightNomalForShadow[2] = 1.0f;

  auto normalize3 = [](float (&vec)[3]) {
    const float len = std::sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    if (len > 0.0f)
    {
      vec[0] /= len;
      vec[1] /= len;
      vec[2] /= len;
    }
  };

  float sat = stState.mMainLightNomal[0] * stState.mMainLightNomal[0]
            + stState.mMainLightNomal[2] * stState.mMainLightNomal[2];
  if (sat < 0.1f)
    sat = 0.1f;
  sat = std::sqrt(sat);
  stState.mLightNomalForShadow[1] = (sat * 5.0f) / 3.0f;

  normalize3(stState.mLightNomalForShadow);
  normalize3(stState.mMainLightNomal);

  float tempLight[3] = {stState.mMainLightNomal[0], 0.0f, stState.mMainLightNomal[2]};
  normalize3(tempLight);

  float cosValue = tempLight[2];
  if (cosValue > 1.0f)
    cosValue = 1.0f;
  if (cosValue < -1.0f)
    cosValue = -1.0f;
  stState.mMainLightYAngle = std::acos(cosValue);
  if (tempLight[0] < 0.0f)
    stState.mMainLightYAngle = -stState.mMainLightYAngle;
}

bool IsInitR3Engine() { return dword_184A77F3C != 0; }
float R3GetTime() { return *reinterpret_cast<float *>(&dword_184A77EE8); }
float R3GetLoopTime() { return *reinterpret_cast<float *>(&dword_184A77EF0); }

IDirect3DDevice8 *GetD3dDevice()
{
  return qword_184A7B2C8;
}

void ResetTexMemSize()
{
  dword_184A79D60 = 0;
}
bool IsExistFile(char *szFileName)
{
  FILE *fp = nullptr;
  if (fopen_s(&fp, szFileName, "rb") == 0 && fp)
  {
    fclose(fp);
    return true;
  }
  return false;
}
unsigned int GetFileSizeAndMergeFile(char *szFileName)
{
  if (!szFileName || !*szFileName)
    return 0;
  if (!qword_184A7B208)
    return GetFileSize(szFileName);
  unsigned int result = qword_184A7B208->GetFileSize(szFileName);
  if (!result)
    return GetFileSize(szFileName);
  return result;
}

__int64 IsExistFileAndMergeFile(char *szFileName)
{
  if (szFileName && *szFileName)
  {
    if (qword_184A7B208 && qword_184A7B208->IsExistFile(szFileName))
      return 1;
    FILE *fp = fopen(szFileName, "rb");
    if (fp)
    {
      fclose(fp);
      return 1;
    }
  }
  return 0;
}

__int64 CalcFileSize(char *pszFileName)
{
  HANDLE hFile = CreateFileA(pszFileName, 0x80000000, 1u, nullptr, 3u, 0x80u, nullptr);
  if (hFile == reinterpret_cast<HANDLE>(-1))
    return 0;

  const DWORD startOffset = SetFilePointer(hFile, 0, nullptr, 0);
  const DWORD endOffset = SetFilePointer(hFile, 0, nullptr, 2u);
  CloseHandle(hFile);
  return static_cast<__int64>(endOffset - startOffset);
}
_R3MATERIAL *LoadMainR3M(char *szFileName)
{
  char *sourceFileName = szFileName;
  char r3xPath[256]{};

  reinterpret_cast<unsigned int *>(&qword_184A79818)[1] = 4278190080;
  dword_184A79920 = 1;
  dword_184A798D0 = -1;
  dword_184A79808 = 0;
  dword_184A79828 = 0;
  dword_184A7980C = 0;
  dword_184A79810 = 0;
  dword_184A79A24 = 0;
  dword_184A79A28 = 0;

  char *dst = r3xPath;
  while ((*dst++ = *szFileName++) != '\0')
    ;
  StripEXT(r3xPath);
  strcat(r3xPath, ".r3x");
  LoadR3X(r3xPath);
  if (qword_184A79DA8)
    ReleaseMainMaterial();

  _R3MATERIAL *result = LoadIndependenceR3M(sourceFileName);
  qword_184A79DA8 = result;
  if (result)
    dword_184A79DB0 = *reinterpret_cast<unsigned int *>(result);
  return result;
}
_R3MATERIAL *LoadSubR3M(char *szFileName)
{
  return LoadIndependenceR3M(szFileName);
}

_R3MATERIAL *LoadIndependenceR3M(char *szFileName)
{
  FILE *fp = fopenMFM(szFileName, "rb");
  if (!fp)
  {
    Warning(szFileName, aAiAaiai_2);
    return nullptr;
  }

  float version = 0.0f;
  fread(&version, 4, 1, fp);
  if (version != 1.1f)
  {
    Warning(szFileName, aAiAaiai_5);
    fclose(fp);
    return nullptr;
  }

  int matNum = 0;
  fread(&matNum, 4, 1, fp);
  auto *mat = static_cast<_R3MATERIAL *>(Dmalloc(sizeof(_R3MATERIAL) * matNum));
  std::memset(mat, 0, sizeof(_R3MATERIAL) * matNum);

  int startId = static_cast<int>(GetNowR3D3DTexCnt()) - 1;
  mat[0].m_iMatNum = matNum;
  if (mat[0].m_iMatNum > 0)
  {
    for (int i = 0; i < mat[0].m_iMatNum; ++i)
    {
      _R3MATERIAL *const material = &mat[i];
      fread(&material->m_dwLayerNum, 4, 1, fp);
      fread(&material->m_dwFlag, 4, 1, fp);
      fread(&material->m_iDetailSurface, 4, 1, fp);
      fread(&material->m_fDetailScale, 4, 1, fp);
      fread(material->m_name, sizeof(material->m_name), 1, fp);
      if (material->m_iDetailSurface != -1)
        material->m_iDetailSurface += startId;
      if (static_cast<int>(material->m_dwLayerNum) > 0)
      {
        for (int j = 0; j < static_cast<int>(material->m_dwLayerNum); ++j)
        {
          fread(&material->m_Layer[j], sizeof(_ONE_LAYER), 1, fp);
          material->m_Layer[j].m_iSurface += startId;
        }
      }
      if (material->m_iDetailSurface != -1)
        material->m_dwFlag |= 2u;
    }
  }

  fclose(fp);
  return mat;
}

_R3MATERIAL *LoadSubMaterial(char *szFileName)
{
  char String[256]{};
  char materialPath[256]{};

  char *src = szFileName;
  char *dst = aTexture;
  while ((*dst++ = *src++) != '\0')
    ;
  strcpy_s(materialPath, szFileName);
  strcat(materialPath, "MainMaterial.mst");

  FILE *fp = fopenMFM(materialPath, "rt");
  if (!fp)
  {
    Warning(materialPath, aAiAaiai_3);
    return nullptr;
  }

  _R3MATERIAL *mat = nullptr;
  int matNum = 0;
  if (fscanf(fp, "%s", String) != -1)
  {
    do
    {
      if (!strcmp(String, "end"))
        break;

      _strupr(String);
      if (!strcmp(String, "*MATERIAL_NUM"))
      {
        fscanf(fp, "%s", String);
        matNum = atoi(String);
        if (matNum >= 1024)
          Error(aAo_2, byte_140883769);
        mat = static_cast<_R3MATERIAL *>(Dmalloc(sizeof(_R3MATERIAL) * matNum));
        std::memset(mat, 0, sizeof(_R3MATERIAL) * matNum);
        fscanf(fp, "%s", String);
        if (matNum > 0)
        {
          for (int i = 0; i < matNum; ++i)
          {
            sub_140502610(&mat[i]);
          }
          for (int i = 0; i < matNum; ++i)
          {
            strcpy_s(materialPath, szFileName);
            fscanf(fp, "%s", String);
            strcat(materialPath, String);
            strcpy_s(mat[i].m_name, sizeof(mat[i].m_name), String);
            strcat(materialPath, ".mst");
            fscanf(fp, "%s", String);
            int matIdx = atoi(String);
            sub_140503190(materialPath, &mat[matIdx]);
          }
        }
      }

      if (!strcmp(String, "*FOG_START"))
      {
        dword_184A79808 = 1;
        fscanf(fp, "%s", String);
        const float fogStart = static_cast<float>(atof(String));
        dword_184A79844 = *reinterpret_cast<const unsigned int *>(&fogStart);
        const float fogEnd = static_cast<float>(atof(String));
        dword_184A79814 = *reinterpret_cast<const unsigned int *>(&fogEnd);
        dword_184A7982C = dword_184A79814;
      }

      if (!strcmp(String, "*FOG_END"))
      {
        dword_184A79808 = 1;
        fscanf(fp, "%s", String);
        const float fogStart = static_cast<float>(atof(String));
        *reinterpret_cast<float *>(&qword_184A79848) = fogStart;
        const float fogEnd = static_cast<float>(atof(String));
        *reinterpret_cast<float *>(&qword_184A79818) = fogEnd;
        dword_184A79834 = *reinterpret_cast<const unsigned int *>(&fogEnd);
        dword_184A79778 = *reinterpret_cast<const unsigned int *>(&fogEnd);
      }

      if (!strcmp(String, "*FOG_COLOR"))
      {
        dword_184A79808 = 1;
        if (*reinterpret_cast<float *>(&qword_184A79818) == 0.0f)
        {
          *reinterpret_cast<float *>(&qword_184A79818) = 5000.0f;
          *reinterpret_cast<float *>(&qword_184A79848) = 5000.0f;
        }
        if (*reinterpret_cast<float *>(&dword_184A79814) == 0.0f)
        {
          *reinterpret_cast<float *>(&dword_184A79814) = 5.0f;
          *reinterpret_cast<float *>(&dword_184A79844) = 5.0f;
        }
        fscanf(fp, "%s", String);
        int r = atoi(String);
        fscanf(fp, "%s", String);
        int g = atoi(String);
        fscanf(fp, "%s", String);
        unsigned int color = static_cast<unsigned int>(atoi(String) | ((g | ((r | 0xFFFFFF00) << 8)) << 8));
        dword_184A7981C = color;
        dword_184A7984C = color;
        dword_184A7983C = color;
      }

      if (!strcmp(String, "*FOG_COLOR2"))
      {
        fscanf(fp, "%s", String);
        int r = atoi(String);
        fscanf(fp, "%s", String);
        int g = atoi(String);
        fscanf(fp, "%s", String);
        dword_184A79840 = static_cast<unsigned int>(((g | ((r | 0xFFFFFF00) << 8)) << 8) | atoi(String));
      }

      if (!strcmp(String, "*FOG_START2"))
      {
        fscanf(fp, "%s", String);
        float v = static_cast<float>(atof(String));
        dword_184A79830 = *reinterpret_cast<unsigned int *>(&v);
      }

      if (!strcmp(String, "*FOG_END2"))
      {
        fscanf(fp, "%s", String);
        float v = static_cast<float>(atof(String));
        dword_184A79838 = *reinterpret_cast<unsigned int *>(&v);
      }

      if (!strcmp(String, "*NO_FOG_SKY"))
        dword_184A7980C = 1;

      if (!strcmp(String, "*FOG_RANGE"))
      {
        fscanf(fp, "%s", String);
        if (!strcmp(String, "TRUE"))
          dword_184A79810 = 1;
      }

      if (!strcmp(String, "*LENS_FLARE_SCALE"))
      {
        float *dstScale = unk_184A798D4;
        float *end = reinterpret_cast<float *>(&dword_184A79914);
        while (dstScale < end)
        {
          fscanf(fp, "%s", String);
          *dstScale++ = static_cast<float>(atof(String));
        }
      }

      if (!strcmp(String, "*LENS_FLARE"))
      {
        fscanf(fp, "%s", String);
        strcpy_s(materialPath, szFileName);
        strcat(materialPath, String);
        dword_184A798D0 = R3GetPreTextureId(materialPath);
        char *state = reinterpret_cast<char *>(&stState);
        size_t idx = 0;
        do
        {
          state[idx + 2143] = materialPath[idx];
        } while (materialPath[idx++]);
      }

      if (!strcmp(String, "*LENS_FLARE_POS"))
      {
        fscanf(fp, "%s", String);
        *reinterpret_cast<float *>(&dword_184A79914) = static_cast<float>(atof(String));
        fscanf(fp, "%s", String);
        *reinterpret_cast<float *>(&dword_184A7991C) = static_cast<float>(atof(String));
        fscanf(fp, "%s", String);
        *reinterpret_cast<float *>(&dword_184A79918) = static_cast<float>(atof(String));
      }

      if (!strcmp(String, "*ENVIRONMENT"))
      {
        fscanf(fp, "%s", String);
        char *state = reinterpret_cast<char *>(&stState);
        size_t idx = 0;
        do
        {
          state[idx + 2355] = String[idx];
        } while (String[idx++]);
      }

      if (!strcmp(String, "*ENVIRONMENT_ID"))
      {
        fscanf(fp, "%s", String);
        dword_184A79A24 = atoi(String);
      }

      if (!strcmp(String, "*ENVIRONMENT2"))
      {
        fscanf(fp, "%s", String);
        char *state = reinterpret_cast<char *>(&stState);
        size_t idx = 0;
        do
        {
          state[idx + 2483] = String[idx];
        } while (String[idx++]);
      }

      if (!strcmp(String, "*ENVIRONMENT_ID2"))
      {
        fscanf(fp, "%s", String);
        dword_184A79A28 = atoi(String);
      }

      _strlwr(String);
      if (!strcmp(String, "*weather"))
      {
        fscanf(fp, "%s", String);
        float v = static_cast<float>(atof(String));
        dword_184A79820 &= ~0xFFu;
        dword_184A79820 |= static_cast<unsigned int>(v * 255.0f);
      }
      if (!strcmp(String, "*haze"))
      {
        fscanf(fp, "%s", String);
        float v = static_cast<float>(atof(String));
        dword_184A79820 &= ~(0xFFu << 8);
        dword_184A79820 |= static_cast<unsigned int>(v * 255.0f) << 8;
      }
      if (!strcmp(String, "*special_weather"))
      {
        fscanf(fp, "%s", String);
        float v = static_cast<float>(atof(String));
        dword_184A79820 &= ~(0xFFu << 16);
        dword_184A79820 |= static_cast<unsigned int>(v * 255.0f) << 16;
      }
      if (!strcmp(String, "*no_sun"))
        dword_184A79824 |= 1u;
      if (!strcmp(String, "*no_sky_color"))
        dword_184A79824 |= 4u;
      if (!strcmp(String, "*use_fog_fog2"))
        dword_184A79824 |= 2u;
    } while (fscanf(fp, "%s", String) != -1);
  }

  if (mat)
    mat->m_iMatNum = matNum;
  fclose(fp);
  return reinterpret_cast<_R3MATERIAL *>(mat);
}

void ReleaseMainMaterial()
{
  if (qword_184A79DA8)
    Dfree(qword_184A79DA8);
  dword_184A79DB0 = 0;
  qword_184A79DA8 = nullptr;
}

void ReleaseSubMaterial(struct _R3MATERIAL *mat)
{
  if (mat)
    Dfree(mat);
}

void AdjustIndependenceR3M(struct _R3MATERIAL *mat, int startId, int newStartId)
{
  if (!mat)
    return;
  int matNum = mat->m_iMatNum;
  if (matNum <= 0)
    return;
  for (int i = 0; i < matNum; ++i)
  {
    _R3MATERIAL *const material = &mat[i];
    if (material->m_iDetailSurface != -1)
      material->m_iDetailSurface = newStartId + material->m_iDetailSurface - startId;
    if (static_cast<int>(material->m_dwLayerNum) > 0)
    {
      for (int j = 0; j < static_cast<int>(material->m_dwLayerNum); ++j)
      {
        material->m_Layer[j].m_iSurface += newStartId - startId;
      }
    }
  }
}
struct R3Texture *R3GetTexInfoR3T(char *szFileName, int flag)
{
  R3Texture *textureTable = GetTextureTable();
  if (!qword_184A79D70)
  {
    qword_184A79D70 = Dmalloc(144 * dword_140978968);
    std::memset(qword_184A79D70, 0, 144LL * dword_140978968);
    qword_184A79D68 = Dmalloc(16 * dword_140978960);
    std::memset(qword_184A79D68, 0, 16LL * dword_140978960);
    textureTable = GetTextureTable();
  }

  int textureCount = dword_140978964;
  if (textureCount >= dword_140978968)
  {
    textureTable =
      static_cast<R3Texture *>(ReAlloc(textureTable, 144 * dword_140978968, 16 * (9 * dword_140978968 + 144)));
    const int oldMaxCount = dword_140978968;
    qword_184A79D70 = textureTable;
    std::memset(&textureTable[dword_140978968], 0, 144uLL * 16);
    textureCount = dword_140978964;
    dword_140978968 = oldMaxCount + 16;
  }

  if ((stR3TexManageFlag & 1) == 0 && textureCount > 1)
  {
    for (int textureIndex = 1; textureIndex < textureCount; ++textureIndex)
    {
      if (std::strcmp(textureTable[textureIndex].mName, szFileName) == 0)
      {
        ++textureTable[textureIndex].mSameCnt;
        return &textureTable[textureIndex];
      }
    }
  }

  if (std::strlen(szFileName) >= 127)
  {
    Warning(aA_3, byte_140883769);
    return nullptr;
  }

  FILE *fp = fopenMFM(szFileName, "rb");
  if (!fp)
  {
    char normalizedName[256]{};
    strcpy_s(normalizedName, szFileName);
    _strlwr(normalizedName);
    StripEXT(normalizedName);
    const size_t len = std::strlen(normalizedName);
    if (len < 3 || normalizedName[len - 3] != 'l' || normalizedName[len - 2] != 'g' || normalizedName[len - 1] != 't')
      Warning(szFileName, aAiAaiai_4);
    return nullptr;
  }

  char header[12]{};
  int texNum = 0;
  fread(header, 4, 1, fp);
  fread(&texNum, 4, 1, fp);
  fclose(fp);

  R3Texture *const entry = &textureTable[dword_140978964];
  entry->mFlag = static_cast<unsigned int>(flag);
  entry->mStartID = dword_14097895C;
  entry->mTexNum = static_cast<unsigned int>(texNum);
  char *dst = entry->mName;
  while ((*dst++ = *szFileName++) != '\0')
    ;

  if (dword_14097895C + static_cast<unsigned int>(texNum) >= dword_140978960)
  {
    unsigned int *newSlots =
      static_cast<unsigned int *>(ReAlloc(qword_184A79D68, 16 * dword_140978960, 16 * (dword_140978960 + texNum)));
    const int oldSlotCount = dword_140978960;
    qword_184A79D68 = newSlots;
    std::memset(&newSlots[4 * oldSlotCount], 0, 16LL * texNum);
    dword_140978960 = oldSlotCount + texNum;
  }

  dword_14097895C += texNum;
  ++dword_140978964;
  return entry;
}

int R3GetPreTextureId(char *szFileName)
{
  R3Texture *textureTable = GetTextureTable();
  R3TextureSlot *textureSlots = nullptr;
  int textureSlotCount = 0;

  if (qword_184A79D70)
  {
    textureSlots = GetTextureSlots();
    textureSlotCount = dword_140978960;
  }
  else
  {
    qword_184A79D70 = Dmalloc(144 * dword_140978968);
    std::memset(qword_184A79D70, 0, 144LL * dword_140978968);
    textureSlots = static_cast<R3TextureSlot *>(Dmalloc(16 * dword_140978960));
    qword_184A79D68 = textureSlots;
    std::memset(textureSlots, 0, 16LL * dword_140978960);
    textureTable = GetTextureTable();
    textureSlotCount = dword_140978960;
  }

  int textureCount = dword_140978964;
  if (textureCount >= dword_140978968)
  {
    textureTable =
      static_cast<R3Texture *>(ReAlloc(textureTable, 144 * dword_140978968, 16 * (9 * dword_140978968 + 144)));
    const int oldMaxCount = dword_140978968;
    qword_184A79D70 = textureTable;
    std::memset(&textureTable[dword_140978968], 0, 144uLL * 16);
    textureSlots = GetTextureSlots();
    textureSlotCount = dword_140978960;
    textureCount = dword_140978964;
    dword_140978968 = oldMaxCount + 16;
  }

  if (std::strlen(szFileName) >= 127)
  {
    Warning(aA_3, byte_140883769);
    return 0;
  }

  if ((stR3TexManageFlag & 1) == 0 && textureCount > 1)
  {
    for (int textureIndex = 1; textureIndex < textureCount; ++textureIndex)
    {
      if (std::strcmp(textureTable[textureIndex].mName, szFileName) == 0)
      {
        ++textureTable[textureIndex].mSameCnt;
        return static_cast<int>(textureTable[textureIndex].mStartID);
      }
    }
  }

  char *dst = textureTable[textureCount].mName;
  while ((*dst++ = *szFileName++) != '\0')
    ;

  if ((stR3TexManageFlag & 1) == 0 && textureCount > 1)
  {
    for (int emptySlotIndex = 1; emptySlotIndex < textureCount; ++emptySlotIndex)
    {
      if (!textureTable[emptySlotIndex].mTexNum && !textureTable[emptySlotIndex].mName[0])
      {
        textureTable[emptySlotIndex].mFlag = 40960;
        textureTable[emptySlotIndex].mStartID = static_cast<unsigned int>(emptySlotIndex);
        textureTable[emptySlotIndex].mTexNum = 1;
        return emptySlotIndex;
      }
    }
  }

  textureTable[textureCount].mFlag = 40960;
  textureTable[textureCount].mStartID = dword_14097895C;
  textureTable[textureCount].mTexNum = 1;
  int nextTextureId = dword_14097895C + 1;
  dword_14097895C = nextTextureId;
  dword_140978964 = textureCount + 1;
  if (nextTextureId >= textureSlotCount)
  {
    R3TextureSlot *expandedSlots =
      static_cast<R3TextureSlot *>(ReAlloc(textureSlots, 16 * textureSlotCount, 16 * (textureSlotCount + 16)));
    qword_184A79D68 = expandedSlots;
    std::memset(&expandedSlots[dword_140978960], 0, sizeof(R3TextureSlot) * 16);
    dword_140978960 += 16;
    nextTextureId = dword_14097895C;
  }

  return nextTextureId - 1;
}

void R3GetPreAniTextureId(char *path, char *name, int *startId, int *num)
{
  int prefixLength = 0;
  char prefix[128]{};
  char extension[32]{};
  char texturePath[256]{};

  do
  {
    if (name[prefixLength] == '.')
      break;
    prefix[prefixLength] = name[prefixLength] == '$' ? '\0' : name[prefixLength];
    ++prefixLength;
  } while (prefixLength < 128);

  extension[0] = '.';
  extension[1] = name[prefixLength + 1];
  extension[2] = name[prefixLength + 2];
  extension[3] = name[prefixLength + 3];
  extension[4] = '\0';

  *num = sub_140501380(path, prefix, extension);
  for (int textureIndex = 1; textureIndex < *num + 1; ++textureIndex)
  {
    strcpy_s(texturePath, path);
    sprintf_s(texturePath, sizeof(texturePath), "%s%s%04d%s", path, prefix, textureIndex, extension);
    const int textureId = R3GetPreTextureId(texturePath);
    if (textureIndex == 1)
      *startId = textureId;
  }
}

void R3ReleasePreTextures()
{
  if (qword_184A79D70)
  {
    Dfree(qword_184A79D70);
    qword_184A79D70 = nullptr;
  }
  dword_140978964 = 1;
  if (qword_184A79D68)
  {
    Dfree(qword_184A79D68);
    qword_184A79D68 = nullptr;
  }
  dword_14097895C = 1;
}

__int64 IsAniTex(char *name)
{
  if (!name)
    return 0;
  unsigned int idx = 0;
  const unsigned __int64 len = std::strlen(name) + 1;
  if (len == 1)
    return 0;
  while (name[idx] != '$')
  {
    if (++idx >= len - 1)
      return 0;
  }
  return 1;
}
void SetNoLodTextere()
{
  if (!dword_184A79DB0)
    return;
  for (unsigned int i = 0; i < dword_184A79DB0; ++i)
  {
    _R3MATERIAL *const material = &qword_184A79DA8[i];
    if ((material->m_dwFlag & 2) != 0)
    {
      SetR3D3DTexture(static_cast<unsigned int>(material->m_iDetailSurface), 2147483648);
      for (unsigned int layerIndex = 0; layerIndex < material->m_dwLayerNum; ++layerIndex)
      {
        SetR3D3DTexture(static_cast<unsigned int>(material->m_Layer[layerIndex].m_iSurface), 2147483648);
      }
    }
  }
}
void LoadR3T(struct R3Texture *pTex)
{
  if (!pTex)
    return;
  FILE *fp = fopenMFM(pTex->mName, "rb");
  if (!fp)
  {
    Warning(pTex->mName, aAiAaiai);
    return;
  }
  float version = 0.0f;
  unsigned int texNum = 0;
  fread(&version, 4, 1, fp);
  fread(&texNum, 4, 1, fp);
  if (version >= 1.1f)
  {
    fseek(fp, static_cast<long>(texNum << 7), SEEK_CUR);
    for (unsigned int i = 0; i < texNum; ++i)
    {
      size_t size = 0;
      fread(&size, 4, 1, fp);
      R3TextureSlot *const slots = GetTextureSlots();
      if (_bittest(reinterpret_cast<const long *>(&slots[i + pTex->mStartID].mFlag), 31))
        slots[i + pTex->mStartID].mTexture = R3LoadDDSFromFP(fp, size, 0, 2048, 2048);
      else
        slots[i + pTex->mStartID].mTexture = R3LoadDDSFromFP(fp, size, dword_184A797D0, 2048, 2048);
    }
    fclose(fp);
  }
  else
  {
    Warning(pTex->mName, aAiAaiaiAiau);
  }
}
unsigned int GetNowTexMemSize()
{
  return dword_184A79D60;
}
__int64 GetNowR3D3DTexCnt()
{
  return dword_14097895C;
}

void SetNowR3D3DTexCnt(int cnt)
{
  if (cnt < dword_14097895C)
    std::memset(&GetTextureSlots()[cnt], 0, sizeof(R3TextureSlot) * (dword_14097895C - cnt));
  dword_14097895C = cnt;
}

__int64 GetNowR3TexCnt()
{
  return dword_140978964;
}

void SetNowR3TexCnt(int cnt)
{
  if (cnt < dword_140978964)
  {
    R3Texture *texture = &GetTextureTable()[cnt];
    __int64 clearCount = dword_140978964 - cnt;
    do
    {
      std::memset(texture++, 0, sizeof(R3Texture));
      --clearCount;
    } while (clearCount);
  }
  dword_140978964 = cnt;
}

void SetR3TexManageFlag(unsigned int flag)
{
  stR3TexManageFlag = flag;
}

__int64 GetR3TexManageFlag()
{
  return stR3TexManageFlag;
}

void R3LoadTextureMem(int id)
{
  R3TextureSlot *const slots = GetTextureSlots();
  R3Texture *const textures = GetTextureTable();
  if (!slots[id].mTexture && dword_140978964 > 1)
  {
    int ownerIndex = 1;
    R3Texture *texture = &textures[1];
    do
    {
      if (texture->mStartID > static_cast<unsigned int>(id))
        break;
      ++ownerIndex;
      ++texture;
    } while (ownerIndex < dword_140978964);

    if (_bittest(reinterpret_cast<const long *>(&textures[ownerIndex].mFlag), 15))
    {
      if (!textures[ownerIndex].mSameCnt)
      {
        const unsigned int mipLevel =
          _bittest(reinterpret_cast<const long *>(&slots[id].mFlag), 31) ? 0 : dword_184A797D0;
        slots[id].mTexture = R3LoadDDS(textures[ownerIndex].mName, mipLevel, 2048, 2048);
      }
    }
    else
    {
      Warning(aR3tAaia_0, byte_140883769);
    }
  }
}

void R3ReleaseTextureMem(int id)
{
  R3TextureSlot *const slots = GetTextureSlots();
  R3Texture *const textures = GetTextureTable();
  if (!slots || id == -1)
    return;
  if (!slots[id].mTexture)
    return;

  int ownerIndex = 1;
  if (dword_140978964 > 1)
  {
    R3Texture *texture = &textures[1];
    do
    {
      if (texture->mStartID > static_cast<unsigned int>(id))
        break;
      ++ownerIndex;
      ++texture;
    } while (ownerIndex < dword_140978964);
  }

  const int refCnt = static_cast<int>(textures[ownerIndex].mSameCnt);
  if (refCnt)
  {
    textures[ownerIndex].mSameCnt = static_cast<unsigned int>(refCnt - 1);
  }
  else
  {
    if (!_bittest(reinterpret_cast<const long *>(&textures[ownerIndex].mFlag), 15))
    {
      Warning(aR3tAaia, byte_140883769);
    }
    textures[ownerIndex].mName[0] = '\0';
    if (slots[id].mTexture)
    {
      IDirect3DTexture8 *const tex = slots[id].mTexture;
      if (tex && tex->__vftable && tex->__vftable->Release)
        tex->__vftable->Release(reinterpret_cast<IDA_IUnknown *>(tex));
      slots[id].mTexture = nullptr;
    }
  }
}

struct IDirect3DTexture8 *R3GetSurface(int id)
{
  if (id <= 0 || id > static_cast<int>(dword_14097895C))
    return nullptr;
  return GetTextureSlots()[id].mTexture;
}

void SetR3D3DTexture(unsigned int id, int flag)
{
  if (id && dword_14097895C > id)
    GetTextureSlots()[id].mFlag = static_cast<unsigned int>(flag);
}

struct IDirect3DTexture8 *R3LoadDDS(char *name, unsigned int mipmap, unsigned int maxX, unsigned int maxY)
{
  unsigned int size = GetFileSizeAndMergeFile(name);
  FILE *fp = fopenMFM(name, "rb");
  if (!fp)
    return nullptr;
  IDirect3DTexture8 *tex = R3LoadDDSFromFP(fp, size, mipmap, maxX, maxY);
  if (!tex)
    tex = R3LoadDDSAndTextureMem(name);
  fclose(fp);
  return tex;
}

struct IDirect3DTexture8 *R3LoadDDSFromFP(FILE *fp, size_t size, unsigned int mipmap, unsigned int maxX, unsigned int maxY)
{
  unsigned int skip = 0;
  auto *buf = static_cast<unsigned int *>(std::malloc(size));
  if (!buf)
    return nullptr;

  fread(buf, 144, 1, fp);
  if (*buf != 542327876)
  {
    sub_1404FFFB0(reinterpret_cast<__int64>(buf));
    if (*buf != 542327876)
    {
      std::free(buf);
      return nullptr;
    }
  }

  unsigned int *ddsWord = buf;
  ddsWord += 2;
  auto *ddsDesc = static_cast<IDA_DDSURFACEDESC2 *>(static_cast<void *>(ddsWord));
  if (ddsDesc->dwMipMapCount)
  {
    skip = static_cast<unsigned int>(GetMipMapSkipSize(ddsDesc, mipmap, maxX, maxY));
    if (skip)
      fseek(fp, skip, SEEK_CUR);
  }

  int dataSize = static_cast<int>(size) - static_cast<int>(skip) - 144;
  unsigned char *payload = reinterpret_cast<unsigned char *>(buf);
  payload += 144;
  fread(payload, dataSize, 1, fp);
  sub_1404FFF30(buf);

  IDirect3DDevice8 *dev = GetD3dDevice();
  unsigned long long outTex = 0;
  int hr = static_cast<int>(D3DXCreateTextureFromFileInMemory_0(dev, buf, static_cast<unsigned int>(dataSize + 144), &outTex));
  std::free(buf);
  if (hr < 0)
    return nullptr;
  dword_184A79D60 += static_cast<unsigned int>(dataSize);
  return reinterpret_cast<IDirect3DTexture8 *>(outTex);
}

struct IDirect3DTexture8 *R3LoadDDSAndTextureMem(char *name)
{
// this is not a stub
  return nullptr;
}
_R3MATERIAL *LoadMainMaterial(char *szFileName)
{
  reinterpret_cast<unsigned int *>(&qword_184A79818)[1] = 4278190080;
  dword_184A79920 = 1;
  dword_184A798D0 = -1;
  dword_184A79808 = 0;
  dword_184A79828 = 0;
  dword_184A7980C = 0;
  dword_184A79810 = 0;
  dword_184A79A24 = 0;
  dword_184A79A28 = 0;
  if (qword_184A79DA8)
    ReleaseMainMaterial();
  _R3MATERIAL *result = LoadSubMaterial(szFileName);
  qword_184A79DA8 = result;
  dword_184A79DB0 = result ? static_cast<unsigned int>(result->m_iMatNum) : 0;
  return result;
}

_R3MATERIAL *GetMainMaterial()
{
  return qword_184A79DA8;
}

unsigned int GetMainMaterialNum()
{
  return dword_184A79DB0;
}

void SetMergeFileManager(CMergeFileManager *pMgr)
{
  qword_184A7B208 = pMgr;
}

void LoadLightMap(char *szFileName)
{
  char lightmapPath[256]{};
  char *dst = lightmapPath;
  while ((*dst++ = *szFileName++) != '\0')
    ;
  qword_184A79DA0 = reinterpret_cast<unsigned long long>(R3GetTexInfoR3T(lightmapPath, 0));
  stLightmap = LoadR3TLightMap(reinterpret_cast<R3Texture *>(qword_184A79DA0), static_cast<D3DFORMAT>(D3DFMT_R5G6B5));
  if (stLightmap)
  {
    const R3Texture *const lightmapTexture = reinterpret_cast<R3Texture *>(qword_184A79DA0);
    dword_184A79D88 = lightmapTexture->mTexNum;
    LightmapTexID = static_cast<int *>(Dmalloc(4 * dword_184A79D88));
    if (dword_184A79D88 > 0)
    {
      for (int textureIndex = 0; textureIndex < static_cast<int>(dword_184A79D88); ++textureIndex)
        LightmapTexID[textureIndex] = static_cast<int>(lightmapTexture->mStartID) + textureIndex;
    }
  }
}

void ReleaseLightMap()
{
  if (LightmapTexID)
  {
    Dfree(LightmapTexID);
    LightmapTexID = nullptr;
    qword_184A79DA0 = 0;
  }
  if (stLightmap)
  {
    if (dword_184A79D88 > 0)
    {
      for (int i = 0; i < static_cast<int>(dword_184A79D88); ++i)
      {
        Dfree(reinterpret_cast<void **>(stLightmap)[i * 2 + 1]);
        Dfree(reinterpret_cast<void **>(stLightmap)[i * 2]);
      }
    }
    dword_184A79D88 = 0;
    Dfree(stLightmap);
    stLightmap = nullptr;
  }
}

struct IDirect3DTexture8 *GetLightMapSurface(int id)
{
  if (LightmapTexID)
    return R3GetSurface(LightmapTexID[id]);
  return nullptr;
}

unsigned int GetLightMapTexSize()
{
  return dword_184A79C54;
}

unsigned int GetLightMapColor(float *const uv, int id)
{
  float u = uv[0];
  if (u <= 0.0f)
    u = 0.0f;
  float v = uv[1];
  if (v <= 0.0f)
    v = 0.0f;
  if (u >= 1.0f)
    u = 1.0f;
  if (v >= 1.0f)
    v = 1.0f;
  uv[0] = u;
  uv[1] = v;
  if (!dword_184A79D88)
    return -1;
  unsigned short *lm = reinterpret_cast<unsigned short **>(stLightmap)[id];
  __int64 idx = static_cast<int>(lm[0] * u) + lm[0] * static_cast<__int64>(static_cast<int>(lm[1] * v));
  unsigned short val = *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(reinterpret_cast<unsigned short **>(lm)[1]) + 2 * idx);
  return ((val >> 11) << 19) | (8 * (val & 0x1F)) | (((val >> 5) & 0x3F) << 10) | 0xFF000000;
}

void CN_SetEnableSky(int bEnable)
{
  stRealSky = bEnable;
}

void R3RestoreAllTextures()
{
  int textureIndex = 1;
  if (dword_140978964 > 1)
  {
    R3Texture *textureTable = GetTextureTable();
    do
    {
      if (_bittest(reinterpret_cast<const long *>(&textureTable[textureIndex].mFlag), 15))
      {
        if (textureTable[textureIndex].mTexNum != 1)
        {
          Error(aAo_1, byte_140883769);
          textureTable = GetTextureTable();
        }
        const unsigned int startId = textureTable[textureIndex].mStartID;
        R3TextureSlot *const slots = GetTextureSlots();
        if (!slots[startId].mTexture)
        {
          IDirect3DTexture8 *dds =
            _bittest(reinterpret_cast<const long *>(&slots[startId].mFlag), 31)
              ? R3LoadDDS(textureTable[textureIndex].mName, 0, 2048, 2048)
              : R3LoadDDS(textureTable[textureIndex].mName, dword_184A797D0, 2048, 2048);
          textureTable = GetTextureTable();
          slots[startId].mTexture = dds;
        }
      }
      ++textureIndex;
    } while (textureIndex < dword_140978964);
  }
  RestoreSystemTexture();
}

void RTMovieCreate(char *szFileName, void *pLevel)
{
// this is not a stub
}

void RTMovieSetState(unsigned int nState)
{
// this is not a stub
}

void R3EnvironmentShakeOff()
{
  stShakeState = 0;
}

void ClearDynamicLight()
{
  stDLightNum = 0;
}
void Error(char *source, char *msg)
{
  const char *sourceText = source;
  char fullMessage[256]{};

  char *dst = fullMessage;
  while ((*dst++ = *source++) != '\0')
    ;

  char *msgDst = &fullMessage[std::strlen(fullMessage) + 1];
  size_t idx = 0;
  while ((msgDst[idx++] = msg[idx - 1]) != '\0')
    ;

  FILE *errorFile = nullptr;
  if (fopen_s(&errorFile, "error_message.txt", "wt") == 0 && errorFile)
  {
    fprintf(errorFile, "%s%s", sourceText, msg);
    fclose(errorFile);
  }

  void (*errorHandler)(char *) = ErrorMessageProc;
  if (!errorHandler)
  {
    MessageBoxA(nullptr, fullMessage, "message", MB_ICONERROR);
    exit(1);
  }

  static const unsigned char asc_140884708[7] = {0x3C, 0x2D, 0xBF, 0xA1, 0xB7, 0xAF, 0x00}; // "<-ì—ì„œ"
  char *suffixDst = &fullMessage[std::strlen(fullMessage) + 1];
  std::memcpy(suffixDst - 1, asc_140884708, sizeof(asc_140884708));
  errorHandler(fullMessage);
}

void Warning(char *source, char *msg)
{
  if (No_warning)
    return;

  char fullMessage[256]{};
  char *dst = fullMessage;
  while ((*dst++ = *source++) != '\0')
    ;
  char *msgDst = &fullMessage[std::strlen(fullMessage) + 1];
  size_t idx = 0;
  while ((msgDst[idx++] = msg[idx - 1]) != '\0')
    ;

  void (*warningHandler)(char *) = WarningMessageProc;
  if (warningHandler)
  {
    static const char kSuffix[] = "\x3C\x2D\xBF\xA1\xB7\xAF"; // "<-ì—ëŸ¬"
    strcat_s(fullMessage, kSuffix);
    warningHandler(fullMessage);
  }
  else
  {
    MessageBoxA(nullptr, fullMessage, "message", 0);
  }
}

void ResetTotalVertexBufferInfo()
{
  dword_184A7B2D8 = 0;
  dword_184A7B2DC = 0;
}

unsigned int GetTotalVertexBufferSize()
{
  return dword_184A7B2D8;
}

unsigned int GetTotalVertexBufferCnt()
{
  return dword_184A7B2DC;
}

void *Dmalloc(int size)
{
  if (!size)
    return nullptr;

  dword_184A78FD4 += static_cast<unsigned int>(size);
  ++dword_184A78FD8;
  auto *allocationBase = static_cast<unsigned int *>(malloc(static_cast<size_t>(size) + 4));
  if (!allocationBase)
    return nullptr;
  *allocationBase = static_cast<unsigned int>(size);
  return allocationBase + 1;
}

void Dfree(void *ptr)
{
  if (!ptr)
    return;

  auto *allocationBase = static_cast<unsigned int *>(ptr) - 1;
  const unsigned int allocationSize = *allocationBase;
  --dword_184A78FD8;
  dword_184A78FD4 -= allocationSize;
  free(allocationBase);
}

void *ReAlloc(void *ptr, unsigned int oldSize, unsigned int newSize)
{
  unsigned int copySize = oldSize;
  if (newSize < copySize)
    copySize = newSize;
  void *dst = Dmalloc(static_cast<int>(newSize));
  if (dst && ptr && copySize)
    std::memcpy(dst, ptr, copySize);
  Dfree(ptr);
  return dst;
}

unsigned int GetDmallocSize()
{
  return dword_184A78FD4;
}

unsigned int GetDmallocCnt()
{
  return dword_184A78FD8;
}

bool IsParticle(char *a1)
{
  _strlwr(a1);
  const size_t len = std::strlen(a1);
  return len >= 3 && a1[len - 3] == 's' && a1[len - 2] == 'p' && a1[len - 1] == 't';
}

CMergeFileManager *GetMergeFileManager()
{
  return qword_184A7B208;
}

FILE *fopenMFM(char *a1, const char *Mode)
{
  const char *mode = Mode;
  char *fileName = a1;
  if (qword_184A7B208)
  {
    CMergeFileManager *mgr = GetMergeFileManager();
    FILE *result = mgr ? mgr->LoadFileOffset(fileName, mode) : nullptr;
    if (result)
      return result;
    Mode = mode;
    a1 = fileName;
  }
  return fopen(a1, Mode);
}

unsigned int GetFileSize(char *a1)
{
  FILE *fp = fopen(a1, "rb");
  if (fp)
  {
    const long startOffset = ftell(fp);
    fseek(fp, 0, SEEK_END);
    const long endOffset = ftell(fp);
    fclose(fp);
    return static_cast<unsigned int>(endOffset - startOffset);
  }
  return 0;
}

__int64 GetTokenFloat(char *a1, float *a2)
{
  const int tokenLength = static_cast<int>(std::strlen(a1));
  char buffer[256]{};
  int bufferLength = 0;

  for (int tokenIndex = 0; tokenIndex < tokenLength; ++tokenIndex)
  {
    const char ch = a1[tokenIndex];
    if (ch == '(')
    {
      bufferLength = 0;
      continue;
    }
    if (ch == ',' || ch == ')')
    {
      buffer[bufferLength] = '\0';
      *a2 = static_cast<float>(std::atof(buffer));
      return tokenIndex + 1;
    }
    buffer[bufferLength++] = ch;
  }

  *a2 = 0.0f;
  return static_cast<unsigned int>(tokenLength);
}

__int64 GetRandOrNum(FILE *Stream, float *a2, float *a3)
{
  char token[256]{};
  char nextToken[256]{};

  fscanf(Stream, "%s", token);
  if (token[0] == 'r' && token[1] == 'a' && token[2] == 'n' && token[3] == 'd')
  {
    int tokenLength = static_cast<int>(std::strlen(token));
    bool hasClosingParen = false;
    while (!hasClosingParen)
    {
      for (int charIndex = 0; charIndex < tokenLength; ++charIndex)
      {
        if (token[charIndex] == ')')
        {
          hasClosingParen = true;
          break;
        }
      }
      if (hasClosingParen)
        break;
      if (fscanf(Stream, "%s", nextToken) == -1 || !std::strcmp(token, "end"))
        Error(byte_140884F48, asc_140884F60);
      strcat_s(token, nextToken);
      tokenLength = static_cast<int>(std::strlen(token));
    }

    const int firstTokenLength = static_cast<int>(GetTokenFloat(token, a2));
    GetTokenFloat(&token[firstTokenLength], a3);
    return 1;
  }

  const float numericValue = static_cast<float>(std::atof(token));
  *a2 = numericValue;
  *a3 = numericValue;
  return 0;
}

__int64 IM_LoadWave(char *a1, unsigned int a2)
{
// this is not a stub
  return 1;
}

void IM_StopWave(unsigned int a1)
{
// this is not a stub
}

void IM_ReleaseWave(unsigned int a1)
{
// this is not a stub
}

void IM_ReleaseAllWaves()
{
}

void IM_SetLoopCntWave(__int64 waveId, unsigned int loopCount)
{
  (void)waveId;
  (void)loopCount;
  // this is not a stub
}

void IM_PlayWave(__int64 waveId, float volume, float pan)
{
  (void)waveId;
  (void)volume;
  (void)pan;
  // this is not a stub
}

void IM_SetWaveVolumeAndPan(__int64 waveId, float volume, float pan)
{
  (void)waveId;
  (void)volume;
  (void)pan;
  // this is not a stub
}

static HRESULT __fastcall Texture_QueryInterface(IDA_IUnknown *self, const IDA_GUID *, void **outObject)
{
if (outObject)
    *outObject = nullptr;
  return 0;
}

static unsigned int __fastcall Texture_AddRef(IDA_IUnknown *self)
{
// this is not a stub
  return 1;
}

static unsigned int __fastcall Texture_Release(IDA_IUnknown *self)
{
  auto *tex = reinterpret_cast<IDirect3DTexture8 *>(self);
  if (tex->m_Data)
    std::free(tex->m_Data);
  std::free(tex);
  return 0;
}

static IDA_IUnknown_vtbl g_TextureVtable = {Texture_QueryInterface, Texture_AddRef, Texture_Release};

static __int64 D3DXCreateTextureFromFileInMemory_0(IDirect3DDevice8 *device, const void *data, unsigned int size, void *outTex)
{
if (!outTex)
    return -1;
  auto **outPtr = reinterpret_cast<IDirect3DTexture8 **>(outTex);
  auto *tex = static_cast<IDirect3DTexture8 *>(std::malloc(sizeof(IDirect3DTexture8)));
  if (!tex)
  {
    *outPtr = nullptr;
    return -1;
  }
  std::memset(tex, 0, sizeof(*tex));
  tex->__vftable = &g_TextureVtable;
  if (size && data)
  {
    tex->m_Data = static_cast<unsigned char *>(std::malloc(size));
    if (!tex->m_Data)
    {
      std::free(tex);
      *outPtr = nullptr;
      return -1;
    }
    std::memcpy(tex->m_Data, data, size);
    tex->m_Size = size;
  }
  *outPtr = tex;
  return 0;
}

static __int64 sub_1404FFFB0(__int64 a1)
{
  auto *data = reinterpret_cast<unsigned int *>(a1);
  __int64 roundIndex = 8;
  __int64 result = 0;
  do
  {
    const unsigned int baseIndex = static_cast<unsigned int>(8 - roundIndex) * 4;
    data[baseIndex] ^= pass_word[baseIndex];
    data[baseIndex + 1] ^= pass_word[baseIndex + 1];
    data[baseIndex + 2] ^= unk_1409788D4[baseIndex];
    result = unk_1409788D8[baseIndex];
    data[baseIndex + 3] ^= static_cast<unsigned int>(result);
    --roundIndex;
  } while (roundIndex);
  return result;
}

static __int64 sub_1404FFF30(unsigned int *a1)
{
  const int fourCc = a1[23];
  __int64 result = 1;
  if (fourCc == 5)
  {
    a1[23] = 4;
  }
  else if (fourCc == 65)
  {
    const int rgbBitCount = a1[25];
    if (rgbBitCount == 16)
    {
      if (a1[29] != 32768)
      {
        a1[26] = 3840;
        result = 0;
      }
    }
    else
    {
      if (rgbBitCount == 32)
      {
        a1[26] = 16711680;
        a1[27] = 65280;
        a1[28] = 255;
        a1[29] = 4278190080;
        result = 0;
      }
    }
  }
  if (!a1[22])
  {
    a1[22] = 32;
    a1[23] = 4;
    return 0;
  }
  return result;
}

static unsigned __int16 sub_1405005A0(unsigned __int16 a1)
{
  if (a1 > 4)
    a1 >>= 1;
  return a1;
}

static int sub_140501380(char *a1, const char *a2, const char *a3)
{
  int textureCount = 1;
  char buffer[256]{};
  while (true)
  {
    strcpy_s(buffer, a1);
    sprintf_s(buffer, sizeof(buffer), "%s%s%04d%s", a1, a2, textureCount, a3);
    FILE *fp = fopen(buffer, "rb");
    if (!fp)
      break;
    fclose(fp);
    if (++textureCount >= 65)
      return 65;
  }
  return textureCount - 1;
}

static void sub_140502610(_R3MATERIAL *material)
{
  if (!material)
    return;

  std::memset(material, 0, sizeof(_R3MATERIAL));
  for (int i = 0; i < 7; ++i)
  {
    _ONE_LAYER *const layer = &material->m_Layer[i];
    layer->m_iSurface = 0;
    layer->m_dwAlphaType = 0;
    layer->m_ARGB = static_cast<unsigned int>(-1);
    layer->m_dwFlag = 0;
  }
}

static int sub_140502690(FILE *Stream, _R3MATERIAL *material, int layerIndex)
{
  _ONE_LAYER *const layer = &material->m_Layer[layerIndex];
  layer->m_ARGB = static_cast<unsigned int>(-1);

  char String[256]{};
  char texturePath[256]{};
  int tileAnimationCounts[4]{};

  int result = fscanf(Stream, "%s", String);
  while (strcmp(String, "}"))
  {
    if (!strcmp(String, "map_name") && !dword_184A79DB4)
    {
      fscanf(Stream, "%s", String);
      strcpy_s(texturePath, aTexture);
      strcat_s(texturePath, String);
      if (!String[0] || !strcmp(String, "NULL"))
      {
        layer->m_iSurface = 0;
      }
      else if (IsAniTex(String))
      {
        layer->m_dwFlag |= 0x1000u;
        R3GetPreAniTextureId(aTexture, String, &layer->m_iSurface, tileAnimationCounts);
        layer->m_iTileAniTexNum = static_cast<unsigned short>(tileAnimationCounts[0]);
      }
      else
      {
        layer->m_iSurface = R3GetPreTextureId(texturePath);
      }
    }
    if (!strcmp(String, "uv_scroll_u"))
    {
      fscanf(Stream, "%s", String);
      layer->m_dwFlag |= 0x10u;
      layer->m_sUVScrollU = static_cast<unsigned short>(static_cast<float>(atof(String)) * 256.0f);
    }
    if (!strcmp(String, "uv_scroll_v"))
    {
      fscanf(Stream, "%s", String);
      layer->m_dwFlag |= 0x20u;
      layer->m_sUVScrollV = static_cast<unsigned short>(static_cast<float>(atof(String)) * 256.0f);
    }
    if (!strcmp(String, "uv_scale"))
    {
      fscanf(Stream, "%s", String);
      layer->m_dwFlag |= 0x80u;
      layer->m_sUVScaleSpeed = 256;
      const unsigned short scale = static_cast<unsigned short>(static_cast<float>(atof(String)) * 256.0f);
      layer->m_sUVScaleStart = scale;
      layer->m_sUVScaleEnd = scale;
    }
    if (!strcmp(String, "uv_scale_speed"))
    {
      fscanf(Stream, "%s", String);
      layer->m_sUVScaleSpeed = static_cast<unsigned short>(static_cast<float>(atof(String)) * 256.0f);
    }
    if (!strcmp(String, "uv_scale_end"))
    {
      fscanf(Stream, "%s", String);
      layer->m_sUVScaleEnd = static_cast<unsigned short>(static_cast<float>(atof(String)) * 256.0f);
    }
    if (!strcmp(String, "uv_rotate"))
    {
      fscanf(Stream, "%s", String);
      layer->m_dwFlag |= 0x40u;
      layer->m_sUVRotate = static_cast<unsigned short>(static_cast<float>(atof(String)) * 256.0f);
    }
    if (!strcmp(String, "uv_lava"))
    {
      fscanf(Stream, "%s", String);
      if (!layer->m_sUVLavaWave)
        layer->m_sUVLavaWave = 256;
      layer->m_dwFlag |= 4u;
      layer->m_sUVLavaSpeed = static_cast<unsigned short>(static_cast<float>(atof(String)) * 256.0f);
    }
    if (!strcmp(String, "uv_lava_wave"))
    {
      fscanf(Stream, "%s", String);
      layer->m_sUVLavaWave = static_cast<unsigned short>(static_cast<float>(atof(String)) * 256.0f);
    }
    if (!strcmp(String, "uv_gradient_alpha_u"))
    {
      fscanf(Stream, "%s", String);
      layer->m_sGradientAlpha |=
        static_cast<unsigned short>(static_cast<int>((static_cast<float>(atof(String)) * 25.0f) + 100.0f)) << 8;
      layer->m_dwFlag |= 0x100u;
    }
    if (!strcmp(String, "uv_gradient_alpha_v"))
    {
      fscanf(Stream, "%s", String);
      layer->m_sGradientAlpha |=
        static_cast<unsigned short>(static_cast<int>((static_cast<float>(atof(String)) * 25.0f) + 100.0f));
      layer->m_dwFlag |= 0x200u;
    }
    if (!strcmp(String, "uv_metal_floor"))
    {
      fscanf(Stream, "%s", String);
      layer->m_dwFlag |= 2u;
      layer->m_sUVMetal = static_cast<unsigned short>(static_cast<float>(atof(String)) * 256.0f);
    }
    if (!strcmp(String, "uv_metal_wall"))
    {
      fscanf(Stream, "%s", String);
      layer->m_dwFlag |= 8u;
      layer->m_sUVMetal = static_cast<unsigned short>(static_cast<float>(atof(String)) * 256.0f);
    }
    if (!strcmp(String, "ani_alpha_flicker"))
    {
      fscanf(Stream, "%s", String);
      layer->m_sANIAlphaFlicker = static_cast<unsigned short>(static_cast<float>(atof(String)) * 256.0f);
      layer->m_dwFlag |= 0x400u;
    }
    if (!strcmp(String, "ani_alpha_flicker_start"))
    {
      fscanf(Stream, "%s", String);
      layer->m_sANIAlphaFlickerAni |=
        static_cast<unsigned short>(static_cast<int>(static_cast<float>(atof(String)) * 255.0f)) << 8;
    }
    if (!strcmp(String, "ani_alpha_flicker_end"))
    {
      fscanf(Stream, "%s", String);
      layer->m_sANIAlphaFlickerAni |=
        static_cast<unsigned short>(static_cast<int>(static_cast<float>(atof(String)) * 255.0f));
    }
    if (!strcmp(String, "ani_tex_speed"))
    {
      fscanf(Stream, "%s", String);
      layer->m_sANITexSpeed = static_cast<unsigned short>(static_cast<float>(atof(String)) * 256.0f);
    }
    if (!strcmp(String, "ani_tex_frame"))
    {
      fscanf(Stream, "%s", String);
      layer->m_dwFlag |= 0x800u;
      layer->m_sANITexFrame = static_cast<unsigned short>(static_cast<float>(atof(String)) * 256.0f);
      if (!layer->m_sANITexSpeed)
        layer->m_sANITexSpeed = 256;
    }
    if (!strcmp(String, "uv_env"))
      layer->m_dwFlag |= 1u;
    if (!strcmp(String, "alpha_sort"))
    {
      fscanf(Stream, "%s", String);
      if (!strcmp(_strupr(String), "TRUE"))
      {
        if (layerIndex)
          Warning(aAlphaSortTrue, byte_140883769);
        else
          layer->m_dwAlphaType |= 0x80000000u;
      }
    }
    if (!strcmp(String, "tex_clamp"))
      layer->m_dwFlag |= 0x10000u;
    if (!strcmp(String, "zwrite"))
      layer->m_dwAlphaType |= 0x2000u;
    if (!strcmp(String, "bump_env"))
      layer->m_dwFlag |= 0x8000u;
    if (!strcmp(String, "type"))
    {
      fscanf(Stream, "%s", String);
      layer->m_dwAlphaType = static_cast<unsigned int>(atoi(String));
    }
    if (!strcmp(String, "alpha"))
    {
      fscanf(Stream, "%s", String);
      layer->m_ARGB &= 0x00FFFFFFu;
      layer->m_ARGB |= static_cast<unsigned int>(atoi(String)) << 24;
    }
    if (!strcmp(String, "color"))
    {
      fscanf(Stream, "%s", String);
      const int r = atoi(String);
      fscanf(Stream, "%s", String);
      const int g = atoi(String);
      fscanf(Stream, "%s", String);
      const int b = atoi(String);
      layer->m_ARGB &= 0xFF000000u;
      layer->m_ARGB |= b | ((g | (r << 8)) << 8);
    }
    result = fscanf(Stream, "%s", String);
  }
  if (!layer->m_sANIAlphaFlickerAni)
    layer->m_sANIAlphaFlickerAni = 255;
  return result;
}

static void sub_140503190(char *a1, _R3MATERIAL *material)
{
  char inLayerBlock = 0;
  FILE *fp = fopenMFM(a1, "rt");
  if (fp)
  {
    if (!dword_184A79DB4)
    {
      for (int i = 0; i < 7; ++i)
        material->m_Layer[i].m_iSurface = -1;
      material->m_iDetailSurface = -1;
      material->m_dwFlag = 0;
    }

    const unsigned int savedTextureDetail = dword_184A797D0;
    char String[256]{};
    if (fscanf(fp, "%s", String) != -1)
    {
      do
      {
        if (!strcmp(String, "end"))
          break;
        if (!strcmp(String, "}"))
        {
          if (!inLayerBlock)
            break;
          inLayerBlock = 0;
        }
        if (!strcmp(String, "light_map"))
        {
          fscanf(fp, "%s", String);
          if (!strcmp(_strupr(String), "TRUE"))
            material->m_dwFlag |= 1u;
        }
        if (!strcmp(String, "sky_sun_light"))
          material->m_dwFlag |= 4u;
        if (!strcmp(String, "sky_night"))
          material->m_dwFlag |= 8u;
        if (!strcmp(String, "sky_day"))
          material->m_dwFlag |= 0x10u;
        if (!strcmp(String, "texture_splatting"))
          material->m_dwFlag |= 0x20u;
        if (!strcmp(String, "layer_num"))
        {
          fscanf(fp, "%s", String);
          material->m_dwLayerNum = static_cast<unsigned int>(atoi(String));
        }
        if (!strcmp(String, "no_lod_texture"))
        {
          material->m_dwFlag |= 2u;
          dword_184A797D0 = 0;
        }
        if (!strcmp(String, "detail_texture"))
        {
          fscanf(fp, "%s", String);
          strcpy_s(a1, 256, aTexture);
          strcat_s(a1, 256, String);
          material->m_dwFlag |= 2u;
          dword_184A797D0 = 0;
          if (!String[0] || !strcmp(String, "NULL"))
            material->m_iDetailSurface = 0;
          else
            material->m_iDetailSurface = R3GetPreTextureId(a1);
          material->m_fDetailScale = 1.0f;
        }
        if (!strcmp(String, "no_collision"))
          material->m_Layer[0].m_dwFlag |= 0x20000u;
        if (!strcmp(String, "water"))
          material->m_Layer[0].m_dwFlag |= 0x60000u;
        if (!strcmp(String, "detail_texture_scale"))
        {
          fscanf(fp, "%s", String);
          material->m_fDetailScale = static_cast<float>(atof(String));
        }
        if (!strcmp(String, "layer"))
        {
          fscanf(fp, "%s", String);
          const int layerIndex = atoi(String);
          sub_140502690(fp, material, layerIndex);
          inLayerBlock = 1;
        }
      } while (fscanf(fp, "%s", String) != -1);
    }
    dword_184A797D0 = savedTextureDetail;
    fclose(fp);
  }
  else
  {
    Warning(a1, aAiAaiai_3);
  }
}

static void LoadR3X(char *a1)
{
  FILE *fp = fopenMFM(a1, "rb");
  if (!fp)
    return;

  float version = 0.0f;
  fread(&version, 4, 1, fp);
  if (version != 1.1f)
  {
    Warning(a1, aAiAaiai_0);
    fclose(fp);
    return;
  }

  _R3X_LOAD_DATA loadData{};
  fread(&loadData, sizeof(loadData), 1, fp);
  fclose(fp);

  if ((loadData.mFlag & 4) != 0)
  {
    dword_184A7983C = static_cast<unsigned int>(loadData.mValue8 >> 32);
    dword_184A79814 = static_cast<unsigned int>(loadData.mValue4);
    qword_184A79818 = static_cast<unsigned long long>(loadData.mValue8);
    dword_184A79778 = static_cast<unsigned int>(loadData.mValue8);
    dword_184A79808 = 1;
    dword_184A79844 = static_cast<unsigned int>(loadData.mValue4);
    qword_184A79848 = static_cast<unsigned long long>(loadData.mValue8);
    dword_184A7982C = static_cast<unsigned int>(loadData.mValue4);
    dword_184A79834 = static_cast<unsigned int>(loadData.mValue8);
  }

  dword_184A79824 = loadData.mData[125];
  if ((loadData.mFlag & 8) != 0)
  {
    dword_184A79840 = loadData.mData[0];
    dword_184A79828 = 1;
    dword_184A79830 = static_cast<unsigned int>(loadData.mValue16);
    dword_184A79838 = static_cast<unsigned int>(loadData.mValue20);
  }

  auto *state = reinterpret_cast<R3EngineStateOverlay *>(static_cast<void *>(&stState));
  if ((loadData.mFlag & 0x10) != 0)
  {
    const char *skyTexture = reinterpret_cast<const char *>(&loadData.mData[23]);
    strcpy_s(state->mSkyTextureName, sizeof(state->mSkyTextureName), skyTexture);
    strcpy_s(byte_184A79850, sizeof(byte_184A79850), skyTexture);
    dword_184A798D0 = R3GetPreTextureId(byte_184A79850);
    dword_184A79914 = loadData.mData[55];
    dword_184A79918 = loadData.mData[56];
    dword_184A7991C = loadData.mData[57];
    for (int i = 0; i < 16; ++i)
      state->mSkyLayerData[i] = loadData.mData[7 + i];
  }

  if ((loadData.mFlag & 0x20) != 0)
  {
    dword_184A79A24 = loadData.mData[90];
    strcpy_s(state->mSecondaryTextureName, sizeof(state->mSecondaryTextureName), reinterpret_cast<const char *>(&loadData.mData[58]));
  }

  if ((loadData.mFlag & 0x20) != 0)
  {
    dword_184A79A28 = loadData.mData[124];
    strcpy_s(state->mTertiaryTextureName, sizeof(state->mTertiaryTextureName), reinterpret_cast<const char *>(&loadData.mData[92]));
  }

  if ((loadData.mFlag & 1) != 0)
    dword_184A7980C = 1;
  if ((loadData.mFlag & 2) != 0)
    dword_184A79810 = 1;
}

static _LIGHTMAP **LoadR3TLightMap(struct R3Texture *a1, D3DFORMAT a2)
{
  unsigned int lightmapIndex = 0;
  unsigned int lightmapCount = 0;
  dword_184A79C54 = 0;
  if (!a1)
    return nullptr;

  FILE *fp = fopen(a1->mName, "rb");
  if (!fp)
    return nullptr;

  float version = 0.0f;
  fread(&version, 4, 1, fp);
  fread(&lightmapCount, 4, 1, fp);
  if (version != 1.1f && version != 1.2f)
  {
    Warning(a1->mName, aAiAaiaiAiau);
    fclose(fp);
    return nullptr;
  }

  auto **list = static_cast<_LIGHTMAP **>(Dmalloc(sizeof(_LIGHTMAP *) * lightmapCount));
  if (!list)
  {
    fclose(fp);
    return nullptr;
  }

  if (lightmapCount)
  {
    auto **cur = list;
    while (true)
    {
      size_t elementSize = 0;
      fread(&elementSize, 4, 1, fp);
      auto *buf = static_cast<unsigned int *>(Dmalloc(static_cast<int>(elementSize)));
      fread(buf, static_cast<unsigned int>(elementSize), 1, fp);
      if (*buf != 542327876)
      {
        Error(a1->mName, aAiR3tAai);
        Dfree(buf);
        fclose(fp);
        return nullptr;
      }
      const unsigned short width = ReadWordAt(buf, 10);
      const unsigned short height = ReadWordAt(buf, 8);
      if (ReadDwordAt(buf, 24))
      {
        Error(a1->mName, aAi);
        Dfree(buf);
        fclose(fp);
        return nullptr;
      }
      const int rgbBitCount = static_cast<int>(ReadDwordAt(buf, 25));
      int d3dFormat = 0;
      if (rgbBitCount == 16)
      {
        d3dFormat = 23;
      }
      else
      {
        if (rgbBitCount != 32)
        {
          Error(a1->mName, aAi);
          Dfree(buf);
          fclose(fp);
          return nullptr;
        }
        d3dFormat = 21;
      }
      if (width > 2048 || height > 2048)
        Error(byte_140883FF8, byte_140883769);
      dword_184A79C54 += width * height * (static_cast<unsigned int>(rgbBitCount) >> 3);
      const int mipWidth = sub_1405005A0(width);
      const int mipHeight = sub_1405005A0(height);
      auto *lm = static_cast<_LIGHTMAP *>(Dmalloc(sizeof(_LIGHTMAP)));
      *cur = lm;
      lm->xl = static_cast<unsigned short>(mipWidth);
      lm->yl = static_cast<unsigned short>(mipHeight);
      lm->buf = static_cast<unsigned short *>(Dmalloc(2 * mipWidth * mipHeight));
      dword_184A79D60 += static_cast<unsigned int>(elementSize);
      if (d3dFormat == 21)
        MakeMipMap(lm->xl, lm->yl, lm->buf,
                   reinterpret_cast<unsigned char *>(buf) + 144);
      else
        MakeMipMap(lm->xl, lm->yl, lm->buf,
                   reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(buf) + 72));

      GetTextureSlots()[lightmapIndex + a1->mStartID].mTexture =
        GetD3DTextureFromBuffer(reinterpret_cast<unsigned char *>(buf), elementSize);
      Dfree(buf);
      ++lightmapIndex;
      ++cur;
      if (lightmapIndex >= lightmapCount)
        break;
    }
  }

  fclose(fp);
  return list;
}

static void RestoreSystemTexture()
{
  unsigned long long dlightTexture = qword_184A79C20;
  if (qword_184A79C20)
  {
    auto *tex = reinterpret_cast<IDirect3DTexture8 *>(qword_184A79C20);
    if (tex && tex->__vftable && tex->__vftable->Release)
      tex->__vftable->Release(reinterpret_cast<IDA_IUnknown *>(tex));
    dlightTexture = qword_184A79C20;
  }
  if (qword_184A79C18)
  {
    char logoPath[] = ".\\system\\logo.dds";
    qword_184A79C18 = reinterpret_cast<unsigned long long>(R3LoadDDS(logoPath, 2, 2048, 2048));
    dlightTexture = qword_184A79C20;
  }
  if (dlightTexture)
  {
    char dlightPath[] = ".\\system\\dlight.dds";
    qword_184A79C20 = reinterpret_cast<unsigned long long>(R3LoadDDS(dlightPath, 2, 2048, 2048));
  }
}

static IDirect3DTexture8 *GetD3DTextureFromBuffer(unsigned __int8 *a1, unsigned int a2)
{
  unsigned long long outTex = 0;
  IDirect3DDevice8 *dev = GetD3dDevice();
  D3DXCreateTextureFromFileInMemory_0(dev, a1, a2, &outTex);
  return reinterpret_cast<IDirect3DTexture8 *>(outTex);
}

static int GetMipMapSkipSize(IDA_DDSURFACEDESC2 *a1, unsigned int a2, unsigned int a3, unsigned int a4)
{
  unsigned int skipSize = 0;
  if (a3 < 256 || a4 < 256)
    Error(aAi_1, byte_140883769);
  unsigned int mipIndex = 0;
  for (mipIndex = 0; mipIndex < a2 || a1->dwWidth > a3 || a1->dwHeight > a4; ++mipIndex)
  {
    if (mipIndex >= a1->dwMipMapCount)
      break;
    const int pitch = a1->lPitch;
    const unsigned int height = a1->dwHeight;
    skipSize += pitch;
    if (height <= 4)
      break;
    a1->dwHeight = height >> 1;
    const unsigned int width = a1->dwWidth;
    if (width <= 4)
      break;
    a1->dwWidth = width >> 1;
    a1->lPitch = pitch / 4;
  }
  a1->dwMipMapCount -= mipIndex;
  return static_cast<int>(skipSize);
}

static void MakeMipMap(unsigned short xl, unsigned short yl, unsigned short *dstBuf, unsigned short *srcBuf)
{
  const int sourceWidth = xl << 1;
  for (int y = 0; y < yl; ++y)
  {
    for (int x = 0; x < xl; ++x)
    {
      int sourceIndex = (y * 2) * sourceWidth + (x << 1);
      unsigned int red = srcBuf[sourceIndex] >> 11;
      unsigned int green = (srcBuf[sourceIndex] >> 5) & 63;
      unsigned int blue = srcBuf[sourceIndex] & 31;

      sourceIndex = (y * 2) * sourceWidth + (x << 1) + 1;
      red += srcBuf[sourceIndex] >> 11;
      green += (srcBuf[sourceIndex] >> 5) & 63;
      blue += srcBuf[sourceIndex] & 31;

      sourceIndex = (y * 2 + 1) * sourceWidth + (x << 1);
      red += srcBuf[sourceIndex] >> 11;
      green += (srcBuf[sourceIndex] >> 5) & 63;
      blue += srcBuf[sourceIndex] & 31;

      sourceIndex = (y * 2 + 1) * sourceWidth + (x << 1) + 1;
      red += srcBuf[sourceIndex] >> 11;
      green += (srcBuf[sourceIndex] >> 5) & 63;
      blue += srcBuf[sourceIndex] & 31;

      red >>= 2;
      green >>= 2;
      blue >>= 2;
      dstBuf[y * xl + x] = static_cast<unsigned short>((red << 11) | (green << 5) | blue);
    }
  }
}

static void MakeMipMap(unsigned short xl, unsigned short yl, unsigned short *dstBuf, unsigned char *srcBuf)
{
  const int sourceWidth = xl << 1;
  for (int y = 0; y < yl; ++y)
  {
    for (int x = 0; x < xl; ++x)
    {
      int sourceIndex = (y * 2) * sourceWidth + (x << 1);
      unsigned int red = srcBuf[sourceIndex * 4];
      unsigned int green = srcBuf[sourceIndex * 4 + 1];
      unsigned int blue = srcBuf[sourceIndex * 4 + 2];

      sourceIndex = (y * 2) * sourceWidth + (x << 1) + 1;
      red += srcBuf[sourceIndex * 4];
      green += srcBuf[sourceIndex * 4 + 1];
      blue += srcBuf[sourceIndex * 4 + 2];

      sourceIndex = (y * 2 + 1) * sourceWidth + (x << 1);
      red += srcBuf[sourceIndex * 4];
      green += srcBuf[sourceIndex * 4 + 1];
      blue += srcBuf[sourceIndex * 4 + 2];

      sourceIndex = (y * 2 + 1) * sourceWidth + (x << 1) + 1;
      red += srcBuf[sourceIndex * 4];
      green += srcBuf[sourceIndex * 4 + 1];
      blue += srcBuf[sourceIndex * 4 + 2];

      red >>= 5;
      green >>= 4;
      blue >>= 5;
      dstBuf[y * xl + x] =
        static_cast<unsigned short>(blue | (32 * (green | (static_cast<unsigned short>(red) << 6))));
    }
  }
}
bool IsProtectItem(int nTableCode)
{
  return nTableCode != 13
      && nTableCode != 17
      && nTableCode != 20
      && nTableCode != 22
      && nTableCode != 19
      && nTableCode != 16
      && nTableCode != 31;
}


unsigned int IsExistItem(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 7:
    {
      _CloakItem_fld *record = static_cast<_CloakItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 12:
    {
      _BagItem_fld *record = static_cast<_BagItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 15:
    {
      _ForceItem_fld *record = static_cast<_ForceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 19:
    {
      _UnitKeyItem_fld *record = static_cast<_UnitKeyItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 32:
    {
      _FIRECRACKER_fld *record = static_cast<_FIRECRACKER_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 35:
    {
      _NPCLink_fld *record = static_cast<_NPCLink_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExist) : 0u;
    }
    case 24:
    case 25:
    case 29:
      return 1u;
    default:
      return 0u;
  }
}


unsigned int IsGroundableItem(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];

  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 7:
    {
      _CloakItem_fld *record = static_cast<_CloakItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 12:
    {
      _BagItem_fld *record = static_cast<_BagItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 15:
    {
      _ForceItem_fld *record = static_cast<_ForceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 17:
    {
      _OreItem_fld *record = static_cast<_OreItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 19:
    {
      _UnitKeyItem_fld *record = static_cast<_UnitKeyItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 24:
    {
      _AnimusItem_fld *record = static_cast<_AnimusItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 29:
      return 0u;
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 32:
    {
      _FIRECRACKER_fld *record = static_cast<_FIRECRACKER_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 35:
    {
      _NPCLink_fld *record = static_cast<_NPCLink_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bGround) : 0u;
    }
    default:
      return 0u;
  }
}

int IsExchangeItem(int nTableCode, int nItemIndex)
{
  CRecordData *table = &s_ptblItemData[nTableCode];
  switch (nTableCode)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    {
      _DfnEquipItem_fld *record = static_cast<_DfnEquipItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 6:
    {
      _WeaponItem_fld *record = static_cast<_WeaponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 8:
    {
      _RingItem_fld *record = static_cast<_RingItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 9:
    {
      _AmuletItem_fld *record = static_cast<_AmuletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 10:
    {
      _BulletItem_fld *record = static_cast<_BulletItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 11:
    {
      _MakeToolItem_fld *record = static_cast<_MakeToolItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 12:
    {
      _BagItem_fld *record = static_cast<_BagItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 13:
    {
      _PotionItem_fld *record = static_cast<_PotionItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 15:
    {
      _ForceItem_fld *record = static_cast<_ForceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 16:
    {
      _BatteryItem_fld *record = static_cast<_BatteryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 17:
    {
      _OreItem_fld *record = static_cast<_OreItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 18:
    {
      _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 19:
    {
      _UnitKeyItem_fld *record = static_cast<_UnitKeyItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 20:
    {
      _BootyItem_fld *record = static_cast<_BootyItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 21:
    {
      _MapItem_fld *record = static_cast<_MapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 22:
    {
      _TOWNItem_fld *record = static_cast<_TOWNItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 23:
    {
      _BattleDungeonItem_fld *record = static_cast<_BattleDungeonItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 24:
    {
      _AnimusItem_fld *record = static_cast<_AnimusItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 25:
    {
      _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 26:
    {
      _TrapItem_fld *record = static_cast<_TrapItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 27:
    {
      _SiegeKitItem_fld *record = static_cast<_SiegeKitItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 28:
    {
      _TicketItem_fld *record = static_cast<_TicketItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 30:
    {
      _RecoveryItem_fld *record = static_cast<_RecoveryItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 31:
    {
      _BoxItem_fld *record = static_cast<_BoxItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 32:
    {
      _FIRECRACKER_fld *record = static_cast<_FIRECRACKER_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 33:
    {
      _UNmannedminer_fld *record = static_cast<_UNmannedminer_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 34:
    {
      _RadarItem_fld *record = static_cast<_RadarItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 35:
    {
      _NPCLink_fld *record = static_cast<_NPCLink_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    case 36:
    {
      _CouponItem_fld *record = static_cast<_CouponItem_fld *>(table->GetRecord(nItemIndex));
      return record ? static_cast<unsigned int>(record->m_bExchange) : 0;
    }
    default:
      return 0;
  }
}


void IOFileWrite_3(const char *pszFileName, unsigned int nLen, const char *pszData)
{
  HANDLE hFile = CreateFileA(pszFileName, 0x40000000u, 1u, nullptr, 4u, 0x80u, nullptr);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    SetFilePointer(hFile, 0, nullptr, 2u);
    DWORD written = 0;
    WriteFile(hFile, pszData, nLen, &written, nullptr);
    CloseHandle(hFile);
  }
}


__time64_t time_19(__int64 *_Time)
{
  return _time64(_Time);
}


bool IsSaveItem(int nTableCode)
{
  return nTableCode != 14 && nTableCode != 29;
}

void D3DXMatrixIdentity_0(D3DXMATRIX *out)
{
  if (!out)
  {
    return;
  }
  MatrixIdentity(reinterpret_cast<float (*)[4]>(out));
}

__int64 D3DXMatrixMultiply_0(D3DXMATRIX *out, const D3DXMATRIX *a, const D3DXMATRIX *b)
{
  if (!out || !a || !b)
  {
    return 0;
  }
  MatrixMultiply(
    reinterpret_cast<float (*)[4]>(out),
    reinterpret_cast<float (*)[4]>(const_cast<D3DXMATRIX *>(a)),
    reinterpret_cast<float (*)[4]>(const_cast<D3DXMATRIX *>(b)));
  return reinterpret_cast<__int64>(out);
}

__int64 D3DXMatrixInverse_0(D3DXMATRIX *out, void *det, const D3DXMATRIX *in)
{
  (void)det;
  if (!out || !in)
  {
    return 0;
  }
  if (!MatrixInvert(
        reinterpret_cast<float (*)[4]>(out),
        reinterpret_cast<float (*)[4]>(const_cast<D3DXMATRIX *>(in))))
  {
    return 0;
  }
  return reinterpret_cast<__int64>(out);
}

void GetYBillboardMatrix(D3DXMATRIX *out, float *const src, float *const dst)
{
  const float dx = dst[0] - src[0];
  if (dx == 0.0f)
  {
    D3DXMatrixIdentity_0(out);
    return;
  }

  const float angle = std::atan((dst[2] - src[2]) / dx);
  D3DXMatrixIdentity_0(out);
  const float c = std::cosf(angle);
  const float s = std::sinf(angle);
  out->_11 = c;
  out->_13 = -s;
  out->_31 = s;
  out->_33 = c;
}

void DrawLightMapGroup(CVertexBuffer *vertexBuffer, _BSP_MAT_GROUP *matGroup)
{
  (void)vertexBuffer;
  IDirect3DDevice8 *const d3d = GetD3dDevice();
  if (!d3d || !matGroup)
  {
    return;
  }

  if (matGroup->MtlId == -1)
  {
    d3d->SetTexture(d3d, 0, nullptr);
    d3d->DrawIndexedPrimitive(d3d, D3DPT_TRIANGLELIST, matGroup->VBMinIndex, matGroup->VCnt, matGroup->IBMinIndex, matGroup->TriNum);
    return;
  }

  GetMainMaterial();
  d3d->SetTexture(d3d, 0, nullptr);
  if (matGroup->LgtId == -1)
  {
    MultiTexOff();
    d3d->SetTextureStageState(d3d, 0, D3DTSS_COLOROP, 5u);
  }
  else
  {
    MultiTexOn();
  }

  d3d->SetTextureStageState(d3d, 0, D3DTSS_COLOROP, 4u);
  d3d->SetTextureStageState(d3d, 1, D3DTSS_COLOROP, 4u);
  d3d->SetTextureStageState(d3d, 0, D3DTSS_COLORARG1, 3u);
  d3d->SetTextureStageState(d3d, 1, D3DTSS_COLOROP, 2u);
  d3d->SetTextureStageState(d3d, 0, D3DTSS_COLOROP, 2u);
  d3d->SetTextureStageState(d3d, 2, D3DTSS_COLOROP, 1u);
  d3d->SetTextureStageState(d3d, 2, D3DTSS_ALPHAOP, 1u);

  d3d->SetTexture(d3d, 0, reinterpret_cast<IDirect3DBaseTexture8 *>(R3GetSurface(matGroup->LgtId)));
  d3d->DrawIndexedPrimitive(d3d, D3DPT_TRIANGLELIST, matGroup->VBMinIndex, matGroup->VCnt, matGroup->IBMinIndex, matGroup->TriNum);
}

void R3ReleaseAllTextures()
{
  int index = 1;
  if (dword_14097895C > 1)
  {
    while (index < static_cast<int>(dword_14097895C))
    {
      R3TextureSlot *const textures = GetTextureSlots();
      if (textures && textures[index].mTexture)
      {
        IDirect3DTexture8 *const texture = textures[index].mTexture;
        if (texture && texture->__vftable && texture->__vftable->Release)
        {
          texture->__vftable->Release(reinterpret_cast<IDA_IUnknown *>(texture));
        }
        textures[index].mTexture = nullptr;
      }
      ++index;
    }
  }
  RestoreSystemTexture();
}

unsigned __int16 GetExcelIndexFromCombineExCheckKey(unsigned int dwCombineExCheckKey)
{
  return static_cast<unsigned __int16>(dwCombineExCheckKey >> 16);
}


