#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#pragma pack(push, 1)
struct _qry_sheet_logout
{
  unsigned int dwAvatorSerial;
  _AVATOR_DATA NewData;
  _AVATOR_DATA OldData;
  bool bCheckLowHigh;
  bool bUpdateRefineCnt;
  unsigned __int8 byRefinedCnt;
  unsigned int dwRefineDate;

  _qry_sheet_logout();
  ~_qry_sheet_logout();
  int size();
};
#pragma pack(pop)
