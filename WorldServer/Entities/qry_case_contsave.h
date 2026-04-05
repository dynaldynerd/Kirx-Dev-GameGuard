#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

struct  _qry_case_contsave
{
  unsigned int dwAvatorSerial;
  _AVATOR_DATA NewData;
  _AVATOR_DATA OldData;
  unsigned __int64 dwCanonicalNewUnitCutTime[4];
  unsigned __int64 dwCanonicalOldUnitCutTime[4];
  bool bUpdateRefineCnt;
  unsigned __int8 byRefinedCnt;
  unsigned int dwRefineDate;

  _qry_case_contsave();
  ~_qry_case_contsave();
  int size();
};
