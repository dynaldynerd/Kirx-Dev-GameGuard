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

void _STORAGE_LIST::_storage_con::empty()
{
  m_bLoad = 0;
  m_bLock = 0;
  m_dwETSerialNumber = 0;
  m_byCsMethod = 0;
}

void _STORAGE_LIST::_storage_con::lock(bool bLock)
{
  m_bLock = bLock;
}

char _STORAGE_LIST::EmptyCon(int n)
{
  if (m_nListCode == 1 && n >= 8)
  {
    return 0;
  }
  if (!m_pStorageList[n].m_bLoad)
  {
    return 0;
  }
  m_pStorageList[n].empty();
  return 1;
}

void _STORAGE_LIST::SetAllEmpty()
{
  for (int index = 0; index < m_nListNum; ++index)
  {
    m_pStorageList[index].empty();
  }
}

void _STORAGE_LIST::SetLock(int n, bool bLock)
{
  m_pStorageList[n].lock(bLock);
}

int _STORAGE_LIST::TransInCon(_storage_con *pCon)
{
  int slotIndex = 0;
  if (m_nListCode == 1)
  {
    slotIndex = pCon->m_byTableCode;
    if (slotIndex >= 8)
    {
      return 255;
    }
    if (m_pStorageList[slotIndex].m_bLoad)
    {
      return 255;
    }
  }
  else
  {
    slotIndex = GetIndexEmptyCon();
    if (slotIndex == 255)
    {
      return 255;
    }
  }

  memcpy_0(&m_pStorageList[slotIndex], pCon, 0x29u);
  m_pStorageList[slotIndex].m_pInList = this;
  m_pStorageList[slotIndex].m_byStorageIndex = static_cast<unsigned __int8>(slotIndex);
  m_pStorageList[slotIndex].m_bLoad = 1;
  m_pStorageList[slotIndex].m_byClientIndex = 0;
  m_pStorageList[slotIndex].m_bLock = 0;
  return slotIndex;
}

int _STORAGE_LIST::GetIndexEmptyCon()
{
  for (int index = 0; index < m_nUsedNum; ++index)
  {
    if (!m_pStorageList[index].m_bLoad)
    {
      return index;
    }
  }
  return 255;
}

int _STORAGE_LIST::GetNumEmptyCon()
{
  unsigned int count = 0;
  for (int index = 0; index < m_nUsedNum; ++index)
  {
    if (!m_pStorageList[index].m_bLoad)
    {
      ++count;
    }
  }
  return static_cast<int>(count);
}

int _STORAGE_LIST::GetIndexFromSerial(unsigned __int16 wSerial)
{
  for (int j = 0; j < m_nUsedNum; ++j)
  {
    if (m_pStorageList[j].m_bLoad && m_pStorageList[j].m_wSerial == wSerial)
    {
      m_pStorageList[j].m_pInList = this;
      m_pStorageList[j].m_byStorageIndex = static_cast<unsigned __int8>(j);
      return j;
    }
  }
  return 255;
}

_STORAGE_LIST::_db_con *_STORAGE_LIST::GetPtrFromSerial(unsigned __int16 wSerial)
{
  const int index = GetIndexFromSerial(wSerial);
  if (index == 255)
  {
    return nullptr;
  }
  return &m_pStorageList[index];
}

char _STORAGE_LIST::AlterCurDur(int n, int nAlter, unsigned __int64 *pdwLeftDur)
{
  if (m_nListCode == 1 && n >= 8)
  {
    return 0;
  }
  if (!m_pStorageList[n].m_bLoad)
  {
    return 0;
  }

  if (nAlter <= 0)
  {
    if (nAlter < 0)
    {
      if (m_pStorageList[n].m_dwDur >= m_pStorageList[n].m_dwDur + nAlter)
      {
        m_pStorageList[n].m_dwDur += nAlter;
      }
      else
      {
        m_pStorageList[n].m_dwDur = 0;
      }
    }
  }
  else if (m_pStorageList[n].m_dwDur <= m_pStorageList[n].m_dwDur + nAlter)
  {
    m_pStorageList[n].m_dwDur += nAlter;
  }
  else
  {
    m_pStorageList[n].m_dwDur = static_cast<unsigned __int64>(-1);
  }

  if (!m_pStorageList[n].m_dwDur)
  {
    EmptyCon(n);
  }
  *pdwLeftDur = m_pStorageList[n].m_dwDur;
  return 1;
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
