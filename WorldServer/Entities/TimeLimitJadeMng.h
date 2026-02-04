#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "TimeLimitJade.h"

class __cppobj TimeLimitJadeMng
{
public:
  static TimeLimitJadeMng *Instance();
  TimeLimitJadeMng();
  virtual ~TimeLimitJadeMng() = default;
  bool Init();
  bool InsertList(unsigned __int16 wIdx, _STORAGE_LIST::_db_con *pkItem);
  bool DeleteList(unsigned __int16 wIdx, _STORAGE_LIST::_db_con *pkItem);
  TimeLimitJade *GetSheet(unsigned __int16 wIndex);
  void Release(unsigned __int16 wIdx);
  void ReleaseAll();

  static TimeLimitJadeMng *_pkInstance;
  TimeLimitJade **_ppkTimeLimitJade;
  CLogFile _kLogSys;
};
