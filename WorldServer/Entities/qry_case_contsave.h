#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

struct __cppobj _qry_case_contsave
{
  unsigned int dwAvatorSerial;
  _AVATOR_DATA NewData;
  _AVATOR_DATA OldData;
  bool bUpdateRefineCnt;
  unsigned __int8 byRefinedCnt;
  unsigned int dwRefineDate;

  _qry_case_contsave();
  ~_qry_case_contsave();
  __int64 size();
};
