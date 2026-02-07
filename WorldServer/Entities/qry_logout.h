#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#pragma pack(push, 1)
struct _qry_case_lobby_logout
{
  struct _reged_list
  {
    unsigned __int8 bySlotIndex;
    unsigned int dwCharSerial;
    char szCharName[17];
    int nLevel;
    unsigned int dwDalant;
    unsigned int dwGold;
  };

  unsigned int dwAccountSerial;
  char szLobbyHistoryFileName[64];
  unsigned __int8 byDBRet;
  int nRegeNum;
  _reged_list RegeList[3];

  _qry_case_lobby_logout();
  void init();
  __int64 size();
};

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
  __int64 size();
};
#pragma pack(pop)
