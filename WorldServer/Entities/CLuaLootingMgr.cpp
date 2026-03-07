#include "pch.h"

#include "CLuaLootingMgr.h"

#include "CItemBox.h"
#include "CMapData.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

#include <cstdlib>
#include <new>

CLuaLooting_Novus_Item::_State CLuaLooting_Novus_Item::_State::ms_cEmpty;
CLuaLootingMgr *CLuaLootingMgr::ms_Instance;

CLuaLooting_Novus_Item::_State::_State() : m_bExist(false) {}

bool CLuaLooting_Novus_Item::_State::operator==(const _State &rhs) const
{
  return m_bExist == rhs.m_bExist;
}

CLuaLooting_Novus_Item::CLuaLooting_Novus_Item()
  : m_Item(),
    m_pMap(nullptr),
    m_wLayerIndex(0),
    m_byCreateType(0),
    m_fLootPos{0.0f, 0.0f, 0.0f},
    m_wLootRange(0),
    m_dwLootCount(0)
{
}

CLuaLooting_Novus_Item::~CLuaLooting_Novus_Item()
{
  // this is not a stub
}

CLuaLootingMgr::CLuaLootingMgr()
{
  m_Loot_Novus_ItemArEx.Alloc(500);
  m_dwAddNodeCount = 0;
}

CLuaLootingMgr::~CLuaLootingMgr() = default;

CLuaLootingMgr *CLuaLootingMgr::Instance()
{
  if (!CLuaLootingMgr::ms_Instance)
  {
    CLuaLootingMgr::ms_Instance = new (std::nothrow) CLuaLootingMgr();
  }
  return CLuaLootingMgr::ms_Instance;
}

void CLuaLootingMgr::Destroy()
{
  if (CLuaLootingMgr::ms_Instance)
  {
    delete CLuaLootingMgr::ms_Instance;
    CLuaLootingMgr::ms_Instance = nullptr;
  }
}

bool CLuaLootingMgr::InitSDM(unsigned int dwLoopLootingCount, unsigned int dwLoopLootingTerm)
{
  m_dwLoopLootingCount = dwLoopLootingCount;
  m_dwLoopLootingTerm = dwLoopLootingTerm;
  m_dwAddNodeCount = 0;
  // this is not a stub
  return true;
}

bool CLuaLootingMgr::AddNovusItem(
  const char *strItemCode,
  CMapData *pMap,
  unsigned __int16 wLayerIndex,
  float *fPos,
  unsigned __int16 wLootRange,
  unsigned int dwOverlapCnt,
  unsigned int dwItemNum,
  unsigned __int8 byCreateType)
{
  if (!pMap || !fPos)
  {
    return false;
  }

  const unsigned int index = US::CArrayEx<CLuaLooting_Novus_Item, CLuaLooting_Novus_Item::_State>::SearchSlotIndex<
    US::CArrayEx<CLuaLooting_Novus_Item, CLuaLooting_Novus_Item::_State>::ORDER_INC>(
    &m_Loot_Novus_ItemArEx,
    &CLuaLooting_Novus_Item::_State::ms_cEmpty);
  if (index == static_cast<unsigned int>(-1))
  {
    return false;
  }

  CLuaLooting_Novus_Item::_State *state = m_Loot_Novus_ItemArEx.GetStateAtPtr(index);
  CLuaLooting_Novus_Item *item = m_Loot_Novus_ItemArEx.GetAtPtr(index);
  if (!state || !item)
  {
    return false;
  }

  unsigned __int8 tableCode = static_cast<unsigned __int8>(-1);
  _base_fld *record = nullptr;
  for (unsigned __int8 j = 0; j < 37; ++j)
  {
    record = g_Main.m_tblItemData[j].GetRecord(strItemCode);
    if (record)
    {
      tableCode = j;
      break;
    }
  }

  unsigned int itemDurPoint = 0;
  if (IsOverLapItem(tableCode))
  {
    itemDurPoint = dwOverlapCnt;
  }
  else
  {
    itemDurPoint = GetItemDurPoint(tableCode, record->m_dwIndex);
  }

  unsigned __int8 defSocketNum = GetDefItemUpgSocketNum(tableCode, record->m_dwIndex);
  unsigned __int8 useSockets = 0;
  if (defSocketNum)
  {
    useSockets = static_cast<unsigned __int8>(rand() % defSocketNum + 1);
  }
  const unsigned int bitSocket = GetBitAfterSetLimSocket(useSockets);

  item->m_Item.m_byTableCode = tableCode;
  item->m_Item.m_wItemIndex = record->m_dwIndex;
  item->m_Item.m_dwDur = itemDurPoint;
  item->m_Item.m_dwLv = bitSocket;
  item->m_byCreateType = byCreateType;
  item->m_dwLootCount = dwItemNum;
  item->m_wLootRange = wLootRange;
  item->m_fLootPos[0] = fPos[0];
  item->m_fLootPos[1] = fPos[1];
  item->m_fLootPos[2] = fPos[2];
  item->m_pMap = pMap;
  item->m_wLayerIndex = wLayerIndex;
  state->m_bExist = true;

  ++m_dwAddNodeCount;
  m_dwNextLootingTime = m_dwLoopLootingTerm + GetLoopTime();
  return true;
}

void CLuaLootingMgr::Loop()
{
  if (!m_dwAddNodeCount || GetLoopTime() < m_dwNextLootingTime)
  {
    return;
  }

  m_dwNextLootingTime = m_dwLoopLootingTerm + GetLoopTime();
  unsigned int loopRemaining = m_dwLoopLootingCount;

  const unsigned int size = m_Loot_Novus_ItemArEx.GetSize();
  for (unsigned int index = 0; index < size; ++index)
  {
    CLuaLooting_Novus_Item::_State *state = m_Loot_Novus_ItemArEx.GetStateAtPtr(index);
    if (!state || !state->m_bExist)
    {
      continue;
    }

    CLuaLooting_Novus_Item *item = m_Loot_Novus_ItemArEx.GetAtPtr(index);
    if (!item)
    {
      continue;
    }

    unsigned int spawnCount = item->m_dwLootCount >= loopRemaining ? loopRemaining : item->m_dwLootCount;
    for (unsigned int j = 0; j < spawnCount; ++j)
    {
      float newPos[3]{0.0f, 0.0f, 0.0f};
      item->m_pMap->GetRandPosInRange(item->m_fLootPos, item->m_wLootRange, newPos);
      CreateItemBox(
        &item->m_Item,
        item->m_byCreateType,
        item->m_pMap,
        item->m_wLayerIndex,
        newPos,
        false,
        nullptr,
        0,
        3u);

      --item->m_dwLootCount;
      --loopRemaining;
    }

    if (!item->m_dwLootCount)
    {
      state->m_bExist = false;
      --m_dwAddNodeCount;
    }

    if (!loopRemaining)
    {
      break;
    }
  }
}
