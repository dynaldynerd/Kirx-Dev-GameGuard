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
  int size();
};

struct _qry_sheet_logout
{
  unsigned int dwAvatorSerial;
  _AVATOR_DATA NewData;
  _AVATOR_DATA OldData;
  unsigned __int64 dwCanonicalNewUnitCutTime[4];
  unsigned __int64 dwCanonicalOldUnitCutTime[4];
  bool bCheckLowHigh;
  bool bUpdateRefineCnt;
  unsigned __int8 byRefinedCnt;
  unsigned int dwRefineDate;

  _qry_sheet_logout();
  ~_qry_sheet_logout();
  int size();
};
#pragma pack(pop)
