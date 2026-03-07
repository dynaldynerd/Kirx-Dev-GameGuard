#include "pch.h"

#include "TimeLimitJade.h"

#include <ctime>

#include "CPlayer.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "ResourceItem_fld.h"
#include "WorldServerUtil.h"

TimeLimitJade::WaitCell::WaitCell()
{
  memset_0(this, 0, sizeof(TimeLimitJade::WaitCell));
}

TimeLimitJade::WaitCell::WaitCell(_STORAGE_LIST::_db_con *pItem)
{
  _pkItem = pItem;
}

TimeLimitJade::WaitCell::WaitCell(_STORAGE_LIST::_db_con *pItem, unsigned __int16 wStartTime, unsigned int dwUseTime)
{
  _pkItem = pItem;
  _wStartTime = wStartTime;
  _dwUseTime = dwUseTime;
}

bool TimeLimitJade::WaitCell::operator==(const WaitCell *rhs) const
{
  return _pkItem == rhs->_pkItem;
}

bool TimeLimitJade::WaitCell::operator>(const WaitCell *rhs) const
{
  return _wStartTime > rhs->_wStartTime;
}

TimeLimitJade::UseCell::UseCell()
{
  memset_0(this, 0, sizeof(TimeLimitJade::UseCell));
}

TimeLimitJade::UseCell::UseCell(_STORAGE_LIST::_db_con *pItem)
{
  _pkItem = pItem;
}

TimeLimitJade::UseCell::UseCell(_STORAGE_LIST::_db_con *pItem, unsigned __int16 wStartTime, unsigned int dwUseTime)
{
  _pkItem = pItem;
  _wStartTime = wStartTime;
  _dwUseTime = dwUseTime;
}

bool TimeLimitJade::UseCell::operator==(const UseCell *rhs) const
{
  return _pkItem == rhs->_pkItem;
}

bool TimeLimitJade::UseCell::operator>(const UseCell *rhs) const
{
  return _dwUseTime > rhs->_dwUseTime;
}

TimeLimitJade::TimeLimitJade(CPlayer *p)
  : _pkOwner(p)
{
}

TimeLimitJade::~TimeLimitJade() = default;

bool TimeLimitJade::Init()
{
  return InitWait() && InitUse();
}

bool TimeLimitJade::InitWait()
{
  return _heapWaitRow.Initialize(100);
}

bool TimeLimitJade::InitUse()
{
  return _heapUseRow.Initialize(100);
}

void TimeLimitJade::Release()
{
  _heapWaitRow.Release();
  _heapUseRow.Release();
}

bool TimeLimitJade::InsertWaitList(_STORAGE_LIST::_db_con *pkItem)
{
  _ResourceItem_fld *record =
    static_cast<_ResourceItem_fld *>(g_Main.m_tblItemData[pkItem->m_byTableCode].GetRecord(pkItem->m_wItemIndex));
  if (!record)
  {
    return false;
  }

  if (record->m_nStartTime < 0 && record->m_nStartTime > 23)
  {
    return false;
  }

  WaitCell cell(pkItem, static_cast<unsigned __int16>(record->m_nStartTime), 60u * record->m_nEffContTime);
  return _heapWaitRow.push(cell);
}

bool TimeLimitJade::InsertUseList(_STORAGE_LIST::_db_con *pkItem, unsigned __int16 dwStart, unsigned int dwEnd)
{
  _pkOwner->SetHaveEffectUseTime(pkItem, 1);
  UseCell cell(pkItem, dwStart, dwEnd);
  return _heapUseRow.push(cell);
}

bool TimeLimitJade::DeleteWaitList(_STORAGE_LIST::_db_con *pkItem)
{
  if (_heapWaitRow.empty())
  {
    return false;
  }

  WaitCell *topCell = _heapWaitRow.top();
  if (!topCell)
  {
    return false;
  }

  if (topCell->_pkItem == pkItem)
  {
    return _heapWaitRow.pop();
  }

  WaitCell cell(pkItem);
  return _heapWaitRow.pop(cell);
}

bool TimeLimitJade::DeleteUseList(_STORAGE_LIST::_db_con *pkItem, bool bItemDel)
{
  if (!bItemDel)
  {
    InsertWaitList(pkItem);
  }

  if (_heapUseRow.empty())
  {
    return false;
  }

  UseCell *topCell = _heapUseRow.top();
  if (!topCell)
  {
    return false;
  }

  if (topCell->_pkItem == pkItem)
  {
    _pkOwner->SetHaveEffectUseTime(pkItem, 0);
    return _heapUseRow.pop();
  }

  UseCell cell(pkItem);
  if (_heapUseRow.pop(cell))
  {
    _pkOwner->SetHaveEffectUseTime(pkItem, 0);
    return true;
  }
  return false;
}

__int64 TimeLimitJade::CheckStartTime()
{
  if (_heapWaitRow.empty())
  {
    return 0;
  }

  __time32_t now[4]{};
  _time32(now);

  for (unsigned int dwIndex = 0; ; ++dwIndex)
  {
    if (static_cast<unsigned __int64>(dwIndex) >= _heapWaitRow.size())
    {
      break;
    }

    WaitCell *cell = _heapWaitRow.pop(dwIndex);
    if (!cell)
    {
      continue;
    }

    const int endHour = static_cast<int>(cell->_wStartTime + cell->_dwUseTime / 3600 - 24);
    const int curHour = GetCurrentHour();

    if (endHour < 0 || cell->_wStartTime <= curHour)
    {
      if (cell->_wStartTime <= curHour)
      {
        const int elapsedHourSec = 3600 * (curHour - cell->_wStartTime);
        const int elapsedMinSec = 60 * GetCurrentMin() + elapsedHourSec;
        const unsigned int elapsedTotal = GetCurrentSec() + elapsedMinSec;
        if (cell->_wStartTime <= curHour && elapsedTotal < cell->_dwUseTime)
        {
          const unsigned int dwEnd = cell->_dwUseTime - elapsedTotal + now[0];
          if (dwEnd > static_cast<unsigned int>(now[0]))
          {
            InsertUseList(cell->_pkItem, cell->_wStartTime, dwEnd);
            DeleteWaitList(cell->_pkItem);
            --dwIndex;
          }
        }
      }
    }
    else if (endHour >= curHour)
    {
      const int useMinutes = cell->_dwUseTime / 60 % 60;
      const int curMin = GetCurrentMin();
      if (endHour != curHour || useMinutes >= curMin)
      {
        const int leftSeconds = 60 * (useMinutes + 60 * (endHour - curHour) - curMin);
        if (leftSeconds > 0)
        {
          const unsigned int dwEnd = now[0] + leftSeconds;
          InsertUseList(cell->_pkItem, cell->_wStartTime, dwEnd);
          DeleteWaitList(cell->_pkItem);
          --dwIndex;
        }
      }
    }
  }

  return 0;
}

__int64 TimeLimitJade::CheckEndTime()
{
  if (_heapUseRow.empty())
  {
    return 0;
  }

  __time32_t now[4]{};
  _time32(now);

  for (int j = 0; ; ++j)
  {
    if (static_cast<unsigned __int64>(j) >= _heapUseRow.size())
    {
      break;
    }

    UseCell *cell = _heapUseRow.top();
    if (cell)
    {
      if (cell->_dwUseTime > static_cast<unsigned int>(now[0]))
      {
        return 0;
      }
      DeleteUseList(cell->_pkItem, false);
    }
  }

  return 0;
}
