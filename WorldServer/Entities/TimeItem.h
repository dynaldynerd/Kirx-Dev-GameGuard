#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CRecordData.h"
#include "TimeItem_fld.h"

#ifndef _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#endif
#include <hash_map>
#include <utility>

class  TimeItem
{
public:
  static TimeItem *Instance();
  static const _TimeItem_fld *FindTimeRec(unsigned int nTbl, int nIdx);

  bool Init();
  bool ReadGoods();
  bool MakeLinkTable(char *szMsg, int nSize);
  bool CheckGoods();

  CLogFile _kLogger;
  CRecordData _kRecTimeItem;

private:
  using TimeItemMap = stdext::hash_map<int, const _TimeItem_fld *, stdext::hash_compare<int, std::less<int>>>;
  static TimeItemMap *_phmapTbl;
};
