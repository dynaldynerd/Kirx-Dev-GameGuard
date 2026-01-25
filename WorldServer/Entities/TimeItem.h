#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CRecordData.h"

class __cppobj TimeItem
{
public:
  static TimeItem *Instance();
  bool Init();
  bool ReadGoods();
  bool MakeLinkTable(char *szMsg, int nSize);
  bool CheckGoods();

  CLogFile _kLogger;
  CRecordData _kRecTimeItem;
};
