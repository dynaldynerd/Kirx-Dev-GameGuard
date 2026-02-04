#include "pch.h"

#include "CUserDB.h"

#include <cstring>

#include "CMapOperation.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

void CUserDB::Init(unsigned __int16 index)
{
  m_idWorld.wIndex = index;
  ParamInit();
  m_tmrCheckPlayMin.BeginTimer(0xEA60);
}

void CUserDB::ParamInit()
{
  m_idWorld.dwSerial = static_cast<unsigned int>(-1);
  m_gidGlobal.dwSerial = static_cast<unsigned int>(-1);
  m_AvatorData.InitData();
  m_AvatorData_bk.InitData();
  for (int j = 0; j < 3; ++j)
  {
    m_RegedList[j].init();
  }
  m_bActive = false;
  m_dwSerial = static_cast<unsigned int>(-1);
  m_bDBWaitState = false;
  m_pDBPushData = nullptr;
  m_bChatLock = false;
  m_bNoneUpdateData = false;
  m_byUserDgr = 0;
  m_bySubDgr = 0;
  m_ss.Init();
  m_dwTotalPlayMin = 0;
  m_bDataUpdate = false;
  m_dwLastContSaveTime = 0;
  m_RadarItemMgr.Init();
  m_byUILock = 0xFF;
  m_byUILock_InitFailCnt = 0xFF;
  m_byUILock_FailCnt = 0xFF;
  m_szUILock_PW[0] = '\0';
  m_szAccount_PW[0] = '\0';
  m_byUILock_HintIndex = 0xFF;
  m_uszUILock_HintAnswer[0] = '\0';
  m_byUILock_InitFindPassFailCount = 0xFF;
  m_byUILockFindPassFailCount = 0;
  std::memset(m_dwRequestMoveCharacterSerialList, 0, sizeof(m_dwRequestMoveCharacterSerialList));
  std::memset(m_dwTournamentCharacterSerialList, 0, sizeof(m_dwTournamentCharacterSerialList));
}

bool CUserDB::Update_Map(CUserDB *self, unsigned __int8 map, float *pos)
{
  if (!g_MapOper.IsExistStdMapID(map))
  {
    return false;
  }

  self->m_AvatorData.dbAvator.m_byMapCode = map;
  self->m_AvatorData.dbAvator.m_fStartPos[0] = pos[0];
  self->m_AvatorData.dbAvator.m_fStartPos[1] = pos[1];
  self->m_AvatorData.dbAvator.m_fStartPos[2] = pos[2];
  return true;
}

void CUserDB::Update_PvpPointLeak(long double dValue)
{
  m_AvatorData.dbSupplement.dPvpPointLeak = dValue;
}

void CUserDB::Update_LastAttBuff(bool bSet)
{
  m_AvatorData.dbSupplement.bLastAttBuff = bSet;
}

bool CUserDB::Update_AlterPvPPoint(long double dNewPoint)
{
  m_AvatorData.dbAvator.m_dPvPPoint = dNewPoint;
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_ItemAdd(
  unsigned __int8 storage,
  unsigned __int8 slot,
  const _STORAGE_LIST::_db_con *pItem,
  bool /*bUpdate*/)
{
  if (!IsStorageRange(storage, slot))
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_ItemAdd(CODE) : storage : %d, slot : %d",
      m_aszAvatorName,
      storage,
      slot);
    return false;
  }

  unsigned __int8 bySlotIndex = static_cast<unsigned __int8>(-1);
  unsigned __int8 byTableCode = static_cast<unsigned __int8>(-1);
  unsigned __int16 wItemIndex = static_cast<unsigned __int16>(-1);

  switch (storage)
  {
    case 0u:
    {
      auto *entry = &m_AvatorData.dbInven.m_List[slot];
      if (!entry->Set(pItem))
      {
        bySlotIndex = entry->Key.bySlotIndex;
        byTableCode = entry->Key.byTableCode;
        wItemIndex = entry->Key.wItemIndex;
      }
      break;
    }
    case 1u:
      if (!m_AvatorData.dbAvator.Set(slot, pItem))
      {
        byTableCode = slot;
        wItemIndex = m_AvatorData.dbAvator.m_EquipKey[slot].zItemIndex;
      }
      break;
    case 2u:
    {
      auto *entry = &m_AvatorData.dbEquip.m_EmbellishList[slot];
      if (!entry->Set(pItem))
      {
        bySlotIndex = entry->Key.bySlotIndex;
        byTableCode = entry->Key.byTableCode;
        wItemIndex = entry->Key.wItemIndex;
      }
      break;
    }
    case 3u:
    {
      auto *entry = &m_AvatorData.dbForce.m_List[slot];
      if (!entry->Set(pItem))
      {
        byTableCode = 15;
        wItemIndex = entry->Key.GetIndex();
      }
      break;
    }
    case 4u:
    {
      auto *entry = &m_AvatorData.dbAnimus.m_List[slot];
      if (!entry->Set(pItem))
      {
        byTableCode = 24;
        wItemIndex = entry->Key.byItemIndex;
      }
      break;
    }
    case 5u:
    {
      auto *entry = &m_AvatorData.dbTrunk.m_List[slot];
      if (!entry->Set(pItem, m_AvatorData.dbAvator.m_byRaceSexCode / 2))
      {
        bySlotIndex = entry->Key.bySlotIndex;
        byTableCode = entry->Key.byTableCode;
        wItemIndex = entry->Key.wItemIndex;
      }
      break;
    }
    case 6u:
    {
      auto *entry = &m_AvatorData.dbPersonalAmineInven.m_List[slot];
      if (!entry->Set(pItem))
      {
        bySlotIndex = entry->Key.bySlotIndex;
        byTableCode = entry->Key.byTableCode;
        wItemIndex = entry->Key.wItemIndex;
      }
      break;
    }
    case 7u:
    {
      auto *entry = &m_AvatorData.dbTrunk.m_ExtList[slot];
      if (!entry->Set(pItem, m_AvatorData.dbAvator.m_byRaceSexCode / 2))
      {
        bySlotIndex = entry->Key.bySlotIndex;
        byTableCode = entry->Key.byTableCode;
        wItemIndex = entry->Key.wItemIndex;
      }
      break;
    }
    default:
      break;
  }

  if (byTableCode == static_cast<unsigned __int8>(-1))
  {
    m_bDataUpdate = true;
    return true;
  }

  g_Main.m_logSystemError.Write(
    "%s:Update_Add(storage:%d, Idx:%d) CUR..(item:%d,%d,%d) ADD..(item:%d,%d)",
    m_aszAvatorName,
    storage,
    slot,
    bySlotIndex,
    byTableCode,
    wItemIndex,
    pItem->m_byTableCode,
    pItem->m_wItemIndex);
  return false;
}

bool CUserDB::Update_ItemDelete(unsigned __int8 storage, unsigned __int8 slot, bool /*bUpdate*/)
{
  if (!IsStorageRange(storage, slot))
  {
    g_Main.m_logSystemError.Write(
      "%s : UpdateItemDelete(CODE) : scode : %d, icode : %d",
      m_aszAvatorName,
      storage,
      slot);
    return false;
  }

  bool released = false;
  switch (storage)
  {
    case 0u:
      released = m_AvatorData.dbInven.m_List[slot].Release();
      break;
    case 1u:
      released = m_AvatorData.dbAvator.Release(slot);
      break;
    case 2u:
      released = m_AvatorData.dbEquip.m_EmbellishList[slot].Release();
      break;
    case 3u:
      released = m_AvatorData.dbForce.m_List[slot].Release();
      break;
    case 4u:
      released = m_AvatorData.dbAnimus.m_List[slot].Release();
      break;
    case 5u:
      released = m_AvatorData.dbTrunk.m_List[slot].Release();
      break;
    case 6u:
      released = m_AvatorData.dbPersonalAmineInven.m_List[slot].Release();
      break;
    case 7u:
      released = m_AvatorData.dbTrunk.m_ExtList[slot].Release();
      break;
    default:
      break;
  }

  if (released)
  {
    m_bDataUpdate = true;
    return true;
  }

  g_Main.m_logSystemError.Write(
    "%s:UpdateItemDelete(storage:%d, Idx:%d)",
    m_aszAvatorName,
    storage,
    slot);
  return false;
}

bool CUserDB::Update_Money(unsigned int dalant, unsigned int gold)
{
  m_AvatorData.dbAvator.m_dwDalant = dalant;
  m_AvatorData.dbAvator.m_dwGold = gold;
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_SFContDelete(unsigned __int8 byContCode, unsigned __int8 bySlotIndex)
{
  if (byContCode < 2)
  {
    if (bySlotIndex < 8)
    {
      if (m_AvatorData.dbSfcont.m_List[byContCode][bySlotIndex].IsFilled())
      {
        m_AvatorData.dbSfcont.m_List[byContCode][bySlotIndex].Init();
        return true;
      }
      g_Main.m_logSystemError.Write(
        "%s : Update_SFContDelete(NOTHING) : code : %d, slot : %d",
        m_aszAvatorName,
        byContCode,
        bySlotIndex);
      return false;
    }
    g_Main.m_logSystemError.Write(
      "%s : Update_SFContDelete(SlotIndex OVER) : code : %dslot : %d",
      m_aszAvatorName,
      byContCode,
      bySlotIndex);
    return false;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_SFContDelete(byContCode OVER) : code : %d, slot : %d",
    m_aszAvatorName,
    byContCode,
    bySlotIndex);
  return false;
}

bool CUserDB::Update_SFContInsert(
  unsigned __int8 byContCode,
  unsigned __int8 bySlotIndex,
  unsigned __int8 byEffectCode,
  unsigned __int16 wEffectIndex,
  unsigned __int8 byLv,
  unsigned __int16 wDurSec)
{
  if (byContCode < 2)
  {
    if (bySlotIndex < 8)
    {
      if (m_AvatorData.dbSfcont.m_List[byContCode][bySlotIndex].IsFilled())
      {
        g_Main.m_logSystemError.Write(
          "%s : Update_SFContInsert(EXIST) : code : %d, slot : %d",
          m_aszAvatorName,
          byContCode,
          bySlotIndex);
        return false;
      }
      m_AvatorData.dbSfcont.m_List[byContCode][bySlotIndex].SetKey(
        0,
        byEffectCode,
        wEffectIndex,
        byLv,
        wDurSec);
      return true;
    }
    g_Main.m_logSystemError.Write(
      "%s : Update_SFContInsert(SlotIndex OVER) : slot : %d",
      m_aszAvatorName,
      bySlotIndex);
    return false;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_SFContInsert(byContCode OVER) : code : %d",
    m_aszAvatorName,
    byContCode);
  return false;
}

bool CUserDB::Update_SFContUpdate(unsigned __int8 byContCode, unsigned __int8 bySlotIndex, unsigned __int16 wTime)
{
  if (byContCode < 2)
  {
    if (bySlotIndex < 8)
    {
      if (m_AvatorData.dbSfcont.m_List[byContCode][bySlotIndex].IsFilled())
      {
        m_AvatorData.dbSfcont.m_List[byContCode][bySlotIndex].SetLeftTime(wTime);
        return true;
      }
      g_Main.m_logSystemError.Write(
        "%s : Update_SFContUpdate(NOTHING) : code : %d, slot : %d",
        m_aszAvatorName,
        byContCode,
        bySlotIndex);
      return false;
    }
    g_Main.m_logSystemError.Write(
      "%s : Update_SFContUpdate(SlotIndex OVER) : slot : %d",
      m_aszAvatorName,
      bySlotIndex);
    return false;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_SFContUpdate(byContCode OVER) : code : %d",
    m_aszAvatorName,
    byContCode);
  return false;
}

bool CUserDB::Update_UnitData(unsigned __int8 bySlotIndex, _UNIT_DB_BASE::_LIST *pData)
{
  if (bySlotIndex < 4)
  {
    if (m_AvatorData.dbUnit.m_List[bySlotIndex].byFrame == 0xFF)
    {
      g_Main.m_logSystemError.Write(
        "%s : Update_UnitData(EXIST) : slot : %d",
        m_aszAvatorName,
        bySlotIndex);
      return false;
    }
    memcpy_0(&m_AvatorData.dbUnit.m_List[bySlotIndex], pData, sizeof(m_AvatorData.dbUnit.m_List[bySlotIndex]));
    m_bDataUpdate = true;
    return true;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_UnitData(SlotIndex OVER) : slot : %d",
    m_aszAvatorName,
    bySlotIndex);
  return false;
}

void _EQUIPKEY::SetRelease()
{
  zItemIndex = -1;
}

bool _EQUIPKEY::IsFilled()
{
  return zItemIndex != -1;
}

void _EQUIPKEY::LoadDBKey(__int16 key)
{
  zItemIndex = key;
}

__int16 _EQUIPKEY::CovDBKey()
{
  return zItemIndex;
}

bool _INVENKEY::IsFilled()
{
  return bySlotIndex != 0xFF || byTableCode != 0xFF || wItemIndex != 0xFFFF;
}

void _INVENKEY::SetRelease()
{
  bySlotIndex = 0xFF;
  byTableCode = 0xFF;
  wItemIndex = 0xFFFF;
}

bool _EMBELLKEY::IsFilled()
{
  return bySlotIndex != 0xFF || byTableCode != 0xFF || wItemIndex != 0xFFFF;
}

void _EMBELLKEY::SetRelease()
{
  bySlotIndex = 0xFF;
  byTableCode = 0xFF;
  wItemIndex = 0xFFFF;
}

void _EMBELLKEY::LoadDBKey(_EMBELLKEY key)
{
  *this = key;
}

__int64 _EMBELLKEY::CovDBKey()
{
  const unsigned int packed =
    static_cast<unsigned int>(bySlotIndex)
    | (static_cast<unsigned int>(byTableCode) << 8)
    | (static_cast<unsigned int>(wItemIndex) << 16);
  return static_cast<__int64>(packed);
}

bool _FORCEKEY::IsFilled()
{
  return dwKey != static_cast<unsigned int>(-1);
}

void _FORCEKEY::SetRelease()
{
  dwKey = static_cast<unsigned int>(-1);
}

void _FORCEKEY::SetKey(unsigned __int8 byItemIndex, unsigned int dwStat)
{
  dwKey = (dwStat & 0xFFFFFFu) | (static_cast<unsigned int>(byItemIndex) << 24);
}

unsigned __int8 _FORCEKEY::GetIndex()
{
  return static_cast<unsigned __int8>(dwKey >> 24);
}

bool _ANIMUSKEY::IsFilled()
{
  return byItemIndex != 0xFF;
}

void _ANIMUSKEY::SetRelease()
{
  byItemIndex = 0xFF;
}

void _ANIMUSKEY::LoadDBKey(unsigned __int8 key)
{
  byItemIndex = key;
}

unsigned __int8 _ANIMUSKEY::CovDBKey()
{
  return byItemIndex;
}

bool _INVEN_DB_BASE::_LIST::Set(const _STORAGE_LIST::_db_con *pItem)
{
  if (Key.IsFilled())
  {
    return false;
  }
  Key.byTableCode = pItem->m_byTableCode;
  Key.wItemIndex = pItem->m_wItemIndex;
  dwDur = pItem->m_dwDur;
  dwUpt = pItem->m_dwLv;
  lnUID = pItem->m_lnUID;
  dwT = pItem->m_dwT;
  byCsMethod = pItem->m_byCsMethod;
  dwLendRegdTime = pItem->m_dwLendRegdTime;
  return true;
}

bool _INVEN_DB_BASE::_LIST::Release()
{
  if (!Key.IsFilled())
  {
    return false;
  }
  Key.SetRelease();
  lnUID = 0;
  dwT = static_cast<unsigned int>(-1);
  byCsMethod = 0;
  return true;
}

bool _EQUIP_DB_BASE::_EMBELLISH_LIST::Set(const _STORAGE_LIST::_db_con *pItem)
{
  if (Key.IsFilled())
  {
    return false;
  }
  Key.byTableCode = pItem->m_byTableCode;
  Key.wItemIndex = pItem->m_wItemIndex;
  wAmount = static_cast<unsigned __int16>(pItem->m_dwDur);
  lnUID = pItem->m_lnUID;
  dwT = pItem->m_dwT;
  byCsMethod = pItem->m_byCsMethod;
  dwLendRegdTime = pItem->m_dwLendRegdTime;
  return true;
}

bool _EQUIP_DB_BASE::_EMBELLISH_LIST::Release()
{
  if (!Key.IsFilled())
  {
    return false;
  }
  Key.SetRelease();
  lnUID = 0;
  dwT = static_cast<unsigned int>(-1);
  byCsMethod = 0;
  return true;
}

bool _FORCE_DB_BASE::_LIST::Set(const _STORAGE_LIST::_db_con *pItem)
{
  if (Key.IsFilled())
  {
    return false;
  }
  if (pItem->m_wItemIndex > 0xFFu)
  {
    return false;
  }
  Key.SetKey(static_cast<unsigned __int8>(pItem->m_wItemIndex), static_cast<unsigned int>(pItem->m_dwDur));
  lnUID = pItem->m_lnUID;
  dwT = pItem->m_dwT;
  byCsMethod = pItem->m_byCsMethod;
  m_dwLendRegdTime = pItem->m_dwLendRegdTime;
  return true;
}

bool _FORCE_DB_BASE::_LIST::Release()
{
  if (!Key.IsFilled())
  {
    return false;
  }
  Key.SetRelease();
  lnUID = 0;
  dwT = static_cast<unsigned int>(-1);
  byCsMethod = 0;
  return true;
}

bool _ANIMUS_DB_BASE::_LIST::Set(const _STORAGE_LIST::_db_con *pItem)
{
  if (Key.IsFilled())
  {
    return false;
  }
  Key.LoadDBKey(static_cast<unsigned __int8>(pItem->m_wItemIndex));
  dwExp = pItem->m_dwDur;
  dwParam = pItem->m_dwLv;
  lnUID = pItem->m_lnUID;
  dwT = pItem->m_dwT;
  byCsMethod = pItem->m_byCsMethod;
  dwLendRegdTime = pItem->m_dwLendRegdTime;
  return true;
}

bool _ANIMUS_DB_BASE::_LIST::Release()
{
  if (!Key.IsFilled())
  {
    return false;
  }
  Key.SetRelease();
  lnUID = 0;
  dwT = static_cast<unsigned int>(-1);
  byCsMethod = 0;
  return true;
}

bool _TRUNK_DB_BASE::_LIST::Set(const _STORAGE_LIST::_db_con *pItem, unsigned __int8 byRaceCode)
{
  if (Key.IsFilled())
  {
    return false;
  }
  Key.byTableCode = pItem->m_byTableCode;
  Key.wItemIndex = pItem->m_wItemIndex;
  dwDur = pItem->m_dwDur;
  dwUpt = pItem->m_dwLv;
  byRace = byRaceCode;
  lnUID = pItem->m_lnUID;
  dwT = pItem->m_dwT;
  byCsMethod = pItem->m_byCsMethod;
  dwLendRegdTime = pItem->m_dwLendRegdTime;
  return true;
}

bool _TRUNK_DB_BASE::_LIST::Release()
{
  if (!Key.IsFilled())
  {
    return false;
  }
  Key.SetRelease();
  lnUID = 0;
  dwT = static_cast<unsigned int>(-1);
  byCsMethod = 0;
  return true;
}

bool _PERSONALAMINE_INVEN_DB_BASE::_LIST::Set(const _STORAGE_LIST::_db_con *pItem)
{
  if (Key.IsFilled())
  {
    return false;
  }
  Key.bySlotIndex = pItem->m_byStorageIndex;
  Key.byTableCode = pItem->m_byTableCode;
  Key.wItemIndex = pItem->m_wItemIndex;
  dwDur = static_cast<unsigned int>(pItem->m_dwDur);
  return true;
}

bool _PERSONALAMINE_INVEN_DB_BASE::_LIST::Release()
{
  if (!Key.IsFilled())
  {
    return false;
  }
  Key.SetRelease();
  return true;
}

bool _REGED::Set(unsigned __int8 bySlot, const _STORAGE_LIST::_db_con *pItem)
{
  if (m_EquipKey[bySlot].IsFilled())
  {
    return false;
  }
  m_EquipKey[bySlot].zItemIndex = pItem->m_wItemIndex;
  m_dwFixEquipLv[bySlot] = pItem->m_dwLv;
  m_lnUID[bySlot] = pItem->m_lnUID;
  m_dwET[bySlot] = pItem->m_dwT;
  m_byCsMethod[bySlot] = pItem->m_byCsMethod;
  m_dwLendRegdTime[bySlot] = pItem->m_dwLendRegdTime;
  return true;
}

bool _REGED::Release(unsigned __int8 bySlot)
{
  if (!m_EquipKey[bySlot].IsFilled())
  {
    return false;
  }
  m_EquipKey[bySlot].SetRelease();
  m_lnUID[bySlot] = 0;
  m_dwET[bySlot] = static_cast<unsigned int>(-1);
  m_byCsMethod[bySlot] = 0;
  return true;
}

void _REGED_AVATOR_DB::Init()
{
  std::memset(this, 0, sizeof(_REGED_AVATOR_DB));
  m_bySlotIndex = static_cast<unsigned __int8>(-1);
  m_dwRecordNum = static_cast<unsigned int>(-1);
  for (int j = 0; j < 8; ++j)
  {
    m_EquipKey[j].SetRelease();
  }
}

void _REGED::init()
{
  Init();
  for (int j = 0; j < 8; ++j)
  {
    m_dwFixEquipLv[j] = 0x0FFFFFFF;
    m_dwItemETSerial[j] = 0;
    m_lnUID[j] = 0;
    m_dwET[j] = static_cast<unsigned int>(-1);
    m_byCsMethod[j] = 0;
    m_dwLendRegdTime[j] = static_cast<unsigned int>(-1);
  }
}

void _SYNC_STATE::Init()
{
  std::memset(this, 0, sizeof(_SYNC_STATE));
}

void _AVATOR_DATA::InitData()
{
  std::memset(this, 0, sizeof(_AVATOR_DATA));
  m_bCristalBattleDateUpdate = 1;
}

