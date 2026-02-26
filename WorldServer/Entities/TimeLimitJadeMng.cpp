#include "pch.h"

#include "TimeLimitJadeMng.h"

#include <new>

#include "GlobalObjects.h"
#include "WorldServerUtil.h"

TimeLimitJadeMng *TimeLimitJadeMng::_pkInstance;

TimeLimitJadeMng::TimeLimitJadeMng()
  : _ppkTimeLimitJade(nullptr)
{
}

TimeLimitJadeMng *TimeLimitJadeMng::Instance()
{
  if (!_pkInstance)
  {
    _pkInstance = new (std::nothrow) TimeLimitJadeMng();
  }
  return _pkInstance;
}

bool TimeLimitJadeMng::Init()
{
  _ppkTimeLimitJade = new (std::nothrow) TimeLimitJade *[MAX_PLAYER];
  if (!_ppkTimeLimitJade)
  {
    return false;
  }

  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    _ppkTimeLimitJade[j] = new (std::nothrow) TimeLimitJade(&g_Player[j]);
    if (!_ppkTimeLimitJade[j] || !_ppkTimeLimitJade[j]->Init())
    {
      ReleaseAll();
      return false;
    }
  }

  char logPath[126]{};
  const unsigned int now = GetKorLocalTime();
  sprintf_s(logPath, "..\\ZoneServerLog\\SystemLog\\PartiallyPaid\\TimeLimitJade%u.log", now);
  _kLogSys.SetWriteLogFile(logPath, 1, 0, 1, 1);
  return true;
}

bool TimeLimitJadeMng::InsertList(unsigned __int16 wIdx, _STORAGE_LIST::_db_con *pkItem)
{
  if (wIdx > MAX_PLAYER)
  {
    return false;
  }
  return _ppkTimeLimitJade[wIdx]->InsertWaitList(pkItem);
}

bool TimeLimitJadeMng::DeleteList(unsigned __int16 wIdx, _STORAGE_LIST::_db_con *pkItem)
{
  if (wIdx > MAX_PLAYER)
  {
    return false;
  }
  if (!pkItem)
  {
    return false;
  }

  if (_ppkTimeLimitJade[wIdx]->DeleteUseList(pkItem, true) || _ppkTimeLimitJade[wIdx]->DeleteWaitList(pkItem))
  {
    return true;
  }

  _kLogSys.Write(
    "TimeLimitJadeMng::DeleteList() >>  - item: %d-%d[%I64u]",
    pkItem->m_byTableCode,
    pkItem->m_wItemIndex,
    pkItem->m_lnUID);
  return false;
}

TimeLimitJade *TimeLimitJadeMng::GetSheet(unsigned __int16 wIndex)
{
  return _ppkTimeLimitJade[wIndex];
}

void TimeLimitJadeMng::Release(unsigned __int16 wIdx)
{
  if (wIdx < MAX_PLAYER)
  {
    _ppkTimeLimitJade[wIdx]->Release();
  }
}

void TimeLimitJadeMng::ReleaseAll()
{
  if (_ppkTimeLimitJade)
  {
    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      delete _ppkTimeLimitJade[j];
      _ppkTimeLimitJade[j] = nullptr;
    }
    delete[] _ppkTimeLimitJade;
    _ppkTimeLimitJade = nullptr;
  }

  if (_pkInstance)
  {
    delete _pkInstance;
    _pkInstance = nullptr;
  }
}
