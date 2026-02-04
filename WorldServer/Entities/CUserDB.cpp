#include "pch.h"

#include "CUserDB.h"

#include <cstring>

#include "CMapOperation.h"
#include "GlobalObjects.h"

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

bool CUserDB::Update_AlterPvPPoint(long double dNewPoint)
{
  m_AvatorData.dbAvator.m_dPvPPoint = dNewPoint;
  m_bDataUpdate = true;
  return true;
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

