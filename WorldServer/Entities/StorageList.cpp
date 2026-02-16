#include "pch.h"

#include "StorageList.h"

#include "CRecordData.h"
#include "GlobalObjects.h"
#include "TimeItem.h"
#include "WorldServerUtil.h"

#include <ctime>
#include <cstdlib>

static unsigned int s_makeLootInitFlag = 0;
static _STORAGE_LIST::_db_con s_makeLootItem{};

_STORAGE_LIST::_STORAGE_LIST()
{
  m_nListNum = 0;
  m_pStorageList = nullptr;
}

void _STORAGE_LIST::SetMemory(_db_con *pList, int nListName, int nListNum, int nUsedNum)
{
  m_pStorageList = pList;
  m_nListCode = nListName;
  m_nListNum = nListNum;
  m_nUsedNum = (nUsedNum != 0) ? nUsedNum : nListNum;

  for (int index = 0; index < m_nListNum; ++index)
  {
    m_pStorageList[index].m_pInList = this;
    m_pStorageList[index].m_byStorageIndex = static_cast<unsigned __int8>(index);
  }
}

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

void _STORAGE_LIST::_db_con::SetSerialNumber(unsigned int dwSN)
{
  if (dwSN)
  {
    m_dwETSerialNumber = dwSN;
  }
  else
  {
    m_dwETSerialNumber = CalcNewSerialNumber();
  }
}

unsigned int _STORAGE_LIST::_db_con::GetSerialNumber() const
{
  return m_dwETSerialNumber;
}

unsigned int _STORAGE_LIST::_db_con::CalcNewSerialNumber()
{
  const int firstRand = rand();
  const int firstHighBit = (rand() % 2) ? 0 : 0x8000;
  const int first = firstHighBit | firstRand;
  const int firstShift = first << 16;
  const int secondRand = rand();
  const int secondHighBit = (rand() % 2) ? 0 : 0x8000;
  const unsigned int serial = static_cast<unsigned int>(secondHighBit | secondRand | firstShift);
  return (serial == 1u) ? 0u : serial;
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

int _STORAGE_LIST::GetNumUseCon() const
{
  return m_nUsedNum;
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

_STORAGE_LIST::_db_con *_STORAGE_LIST::GetPtrFromItemCode(char *pwszItemCode)
{
  for (int j = 0; j < this->m_nUsedNum; ++j)
  {
    if (this->m_pStorageList[j].m_bLoad)
    {
      _base_fld *record =
        g_Main.m_tblItemData[this->m_pStorageList[j].m_byTableCode].GetRecord(
          this->m_pStorageList[j].m_wItemIndex);
      if (record)
      {
        if (!strcmp_0(record->m_strCode, pwszItemCode))
        {
          return &this->m_pStorageList[j];
        }
      }
    }
  }
  return nullptr;
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

char _STORAGE_LIST::UpdateCurDur(int n, int nUpdate)
{
  if (m_nListCode != 3)
  {
    return 0;
  }
  if (!m_pStorageList[n].m_bLoad)
  {
    return 0;
  }
  m_pStorageList[n].m_dwDur = nUpdate;
  return 1;
}

char _STORAGE_LIST::SetUseListNum(int nUsedNum)
{
  if (m_nUsedNum > m_nListNum)
  {
    return 0;
  }
  m_nUsedNum = nUsedNum;
  return 1;
}

char _STORAGE_LIST::GradeUp(int n, unsigned int dwUptInfo)
{
  m_pStorageList[n].m_dwLv = dwUptInfo;
  return 1;
}

char _STORAGE_LIST::GradeDown(int n, unsigned int dwUptInfo)
{
  m_pStorageList[n].m_dwLv = dwUptInfo;
  return 1;
}

char _STORAGE_LIST::SetGrade(int n, unsigned __int8 byLv, unsigned int dwUptInfo)
{
m_pStorageList[n].m_dwLv = dwUptInfo;
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
