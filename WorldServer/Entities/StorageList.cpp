#include "pch.h"

#include "StorageList.h"

#include "TimeItem.h"
#include "WorldServerUtil.h"

#include <ctime>

static unsigned int s_makeLootInitFlag = 0;
static _STORAGE_LIST::_db_con s_makeLootItem{};

void _STORAGE_LIST::_storage_con::Init()
{
  memset_0(this, 0, sizeof(_STORAGE_LIST::_storage_con));
  m_dwLv = 0xFFFFFFF;
}

void _STORAGE_LIST::_db_con::Init()
{
  m_pInList = nullptr;
  m_byStorageIndex = static_cast<unsigned __int8>(-1);
  _STORAGE_LIST::_storage_con::Init();
}

_STORAGE_LIST::_db_con *MakeLoot(unsigned __int8 byTableCode, unsigned __int16 wItemIndex)
{
  unsigned int dwExp = GetItemDurPoint(byTableCode, wItemIndex);
  unsigned int dwExpLevel = 0xFFFFFFF;

  const unsigned __int8 itemKindCode = GetItemKindCode(byTableCode);
  if (itemKindCode)
  {
    if (itemKindCode != 1)
    {
      return nullptr;
    }
    dwExpLevel = GetMaxParamFromExp(wItemIndex, dwExp);
  }
  else
  {
    unsigned __int8 defSocketNum = GetDefItemUpgSocketNum(byTableCode, wItemIndex);
    unsigned __int8 limSocketNum = 0;
    if (defSocketNum)
    {
      limSocketNum = static_cast<unsigned __int8>(rand() % defSocketNum + 1);
    }
    dwExpLevel = GetBitAfterSetLimSocket(limSocketNum);
  }

  if ((s_makeLootInitFlag & 1u) == 0)
  {
    s_makeLootInitFlag |= 1u;
  }

  s_makeLootItem.Init();
  s_makeLootItem.m_byTableCode = byTableCode;
  s_makeLootItem.m_wItemIndex = wItemIndex;
  s_makeLootItem.m_dwDur = dwExp;
  s_makeLootItem.m_dwLv = dwExpLevel;

  const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(byTableCode, wItemIndex);
  if (timeRec && timeRec->m_nCheckType)
  {
    __time32_t now[5];
    _time32(now);
    s_makeLootItem.m_byCsMethod = timeRec->m_nCheckType;
    s_makeLootItem.m_dwT = timeRec->m_nUseTime + now[0];
    s_makeLootItem.m_dwLendRegdTime = now[0];
  }

  return &s_makeLootItem;
}
