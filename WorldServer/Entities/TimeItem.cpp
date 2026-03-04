#include "pch.h"

#include "TimeItem.h"

#include "KorLocalTime.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

#include <cstdio>
#include <cstring>

TimeItem *TimeItem::_pkInstance = nullptr;
TimeItem::TimeItemMap *TimeItem::_phmapTbl;

TimeItem::TimeItem()
{
}

TimeItem *TimeItem::Instance()
{
  if (!_pkInstance)
  {
    _pkInstance = new TimeItem();
  }
  return _pkInstance;
}

const _TimeItem_fld *TimeItem::FindTimeRec(unsigned int nTbl, int nIdx)
{
  if (nTbl > 0x24)
  {
    return nullptr;
  }
  TimeItemMap &map = _phmapTbl[nTbl];
  auto it = map.find(nIdx);
  if (it == map.end())
  {
    return nullptr;
  }
  return it->second;
}

bool TimeItem::Init()
{
  CreateDirectoryA("..\\ZoneServerLog\\SystemLog", nullptr);

  char buffer[256]{};
  const unsigned int now = GetKorLocalTime();
  sprintf_s(buffer, sizeof(buffer), "%s\\TimeItem_%u.log", "..\\ZoneServerLog\\SystemLog", now);
  _kLogger.SetWriteLogFile(buffer, 1, 0, 1, 1);

  return ReadGoods();
}

bool TimeItem::ReadGoods()
{
  char errMsg[144]{};
  if (!_kRecTimeItem.ReadRecord(".\\Script\\TimerItem.dat", 0x8C, errMsg))
  {
    MyMessageBox("TimeItem", errMsg);
    return false;
  }

  if (!MakeLinkTable(errMsg, static_cast<int>(sizeof(errMsg))))
  {
    MyMessageBox("TimeItem", errMsg);
    g_Main.m_logLoadingError.Write("Failed TimeItem::MakeLinkTable()");
    return false;
  }

  return CheckGoods();
}

bool TimeItem::MakeLinkTable(char *szMsg, int nSize)
{
  _phmapTbl = new TimeItemMap[37];

  for (int n = 0;; ++n)
  {
    const int recordNum = _kRecTimeItem.GetRecordNum();
    if (n >= recordNum)
    {
      break;
    }
    auto *record = static_cast<_TimeItem_fld *>(_kRecTimeItem.GetRecord(n));
    if (!record)
    {
      strcpy_s(szMsg, nSize, "Wrong index of time item\r\n");
      return false;
    }

    const int itemTableCode = GetItemTableCode(record->m_strLendItemCode);
    if (itemTableCode == -1)
    {
      strcpy_s(szMsg, nSize, "Wrong string code[_TimeItem_fld::m_strLendItemCode]\r\n");
      return false;
    }

    _base_fld *itemRecord = g_Main.m_tblItemData[itemTableCode].GetRecord(record->m_strLendItemCode, 7);
    if (!itemRecord)
    {
      strcpy_s(szMsg, nSize, "Wrong string code[_TimeItem_fld::m_strLendItemCode]-NonExist\r\n");
      return false;
    }

    _phmapTbl[itemTableCode].insert(std::make_pair(static_cast<int>(itemRecord->m_dwIndex), record));
  }

  return true;
}

bool TimeItem::CheckGoods()
{
  for (int n = 0;; ++n)
  {
    const int recordNum = _kRecTimeItem.GetRecordNum();
    if (n >= recordNum)
    {
      break;
    }

    _TimeItem_fld *record = static_cast<_TimeItem_fld *>(_kRecTimeItem.GetRecord(n));
    unsigned __int8 itemTableCode = static_cast<unsigned __int8>(GetItemTableCode(record->m_strLendItemCode));
    _base_fld *itemRecord = g_Main.m_tblItemData[itemTableCode].GetRecord(record->m_strLendItemCode, 7);
    if (!itemRecord)
    {
      MyMessageBox("TimeItem", "Wrong Code : %s", record->m_strLendItemCode);
      return false;
    }
    if (!IsTimeItem(itemTableCode, static_cast<int>(itemRecord->m_dwIndex)))
    {
      MyMessageBox("TimeItem", "Is not Lend item : %s", record->m_strLendItemCode);
      return false;
    }
    const char *itemKorName = GetItemKorName(itemTableCode, static_cast<int>(itemRecord->m_dwIndex));
    _kLogger.Write(
      "[%s(%s)], method:%d, lentime:%d",
      itemKorName,
      itemRecord->m_strCode,
      record->m_nCheckType,
      record->m_nUseTime);
  }

  return true;
}
