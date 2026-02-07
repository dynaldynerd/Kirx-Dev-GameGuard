#include "pch.h"

#include "LendItemMng.h"

#include <cstdio>

#include "GlobalObjects.h"
#include "TimeItem.h"

LendItemMng *LendItemMng::_pkInstance = nullptr;

LendItemMng::LendItemMng()
  : _ppkLendItem(nullptr)
{
}

LendItemMng::~LendItemMng() = default;

LendItemMng *LendItemMng::Instance()
{
  if (!_pkInstance)
  {
    _pkInstance = new LendItemMng();
  }
  return _pkInstance;
}

bool LendItemMng::Initialize()
{
  _ppkLendItem = new LendItemSheet *[MAX_PLAYER];
  if (!_ppkLendItem)
  {
    return false;
  }

  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    _ppkLendItem[j] = new LendItemSheet(&g_Player[j]);
    if (!_ppkLendItem[j] || !_ppkLendItem[j]->Initialzie())
    {
      ReleaseAll();
      return false;
    }
  }

  char logPath[126]{};
  const unsigned int now = GetKorLocalTime();
  sprintf_s(logPath, "..\\ZoneServerLog\\SystemLog\\PartiallyPaid\\LendLog%d.log", now);
  _kLogSys.SetWriteLogFile(logPath, 1, 0, 1, 1);
  return true;
}

bool LendItemMng::InsertLink(unsigned __int16 wIdx, unsigned __int8 byStorageCode, _STORAGE_LIST::_db_con *pkItem)
{
  if (wIdx >= MAX_PLAYER)
  {
    return false;
  }

  const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(pkItem->m_byTableCode, pkItem->m_wItemIndex);
  if (timeRec)
  {
    if (timeRec->m_nCheckType)
    {
      return _ppkLendItem[wIdx]->InsertLink(byStorageCode, pkItem);
    }
    _kLogSys.Write( "[NO_NELD]InsertLink() >> StorageCode:%d - %I64u", byStorageCode, pkItem->m_lnUID);
    return false;
  }

  _kLogSys.Write( "[NO_ITEM]InsertLink() >> StorageCode:%d - %I64u", byStorageCode, pkItem->m_lnUID);
  return false;
}

bool LendItemMng::DeleteLink(unsigned __int16 wIdx, unsigned __int8 byStorageCode, _STORAGE_LIST::_db_con *pkItem)
{
  if (wIdx < MAX_PLAYER)
  {
    const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(pkItem->m_byTableCode, pkItem->m_wItemIndex);
    if (timeRec)
    {
      if (timeRec->m_nCheckType)
      {
        if (_ppkLendItem[wIdx]->DeleteLink(byStorageCode, pkItem))
        {
          return true;
        }
        _kLogSys.Write( "DeleteLink() >> StorageCode:%d - %I64u", byStorageCode, pkItem->m_lnUID);
        return false;
      }
      _kLogSys.Write( "[NO_NELD]DeleteLink() >> StorageCode:%d - %I64u", byStorageCode, pkItem->m_lnUID);
      return false;
    }
    _kLogSys.Write( "[NO_CASH]DeleteLink() >> StorageCode:%d - %I64u", byStorageCode, pkItem->m_lnUID);
    return false;
  }

  _kLogSys.Write(
    "DeleteLink() >> (wIdx >= MAXPLAYER) - [%d, byStorageCode:%d]",
    wIdx,
    byStorageCode);
  return false;
}

LendItemSheet *LendItemMng::GetSheet(unsigned __int16 wIdx)
{
  return _ppkLendItem[wIdx];
}

void LendItemMng::Release(unsigned __int16 wIdx)
{
  if (wIdx < MAX_PLAYER)
  {
    _ppkLendItem[wIdx]->Release();
  }
}

void LendItemMng::ReleaseAll()
{
  if (_ppkLendItem)
  {
    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      delete _ppkLendItem[j];
      _ppkLendItem[j] = nullptr;
    }
    delete[] _ppkLendItem;
    _ppkLendItem = nullptr;
  }

  if (_pkInstance)
  {
    delete _pkInstance;
    _pkInstance = nullptr;
  }
}
