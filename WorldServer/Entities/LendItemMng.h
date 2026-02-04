#pragma once

#include "IdaCompat.h"
#include "GlobalObjectDefs.h"
#include "CLogFile.h"
#include "LendItemSheet.h"

class __cppobj LendItemMng
{
public:
  LendItemMng();
  ~LendItemMng();

  static LendItemMng *Instance();
  bool Initialize();
  bool InsertLink(unsigned __int16 wIdx, unsigned __int8 byStorageCode, _STORAGE_LIST::_db_con *pkItem);
  bool DeleteLink(unsigned __int16 wIdx, unsigned __int8 byStorageCode, _STORAGE_LIST::_db_con *pkItem);
  LendItemSheet *GetSheet(unsigned __int16 wIdx);
  void Release(unsigned __int16 wIdx);
  void ReleaseAll();

private:
  static LendItemMng *_pkInstance;
  LendItemSheet **_ppkLendItem = nullptr;
  CLogFile _kLogSys;
};
