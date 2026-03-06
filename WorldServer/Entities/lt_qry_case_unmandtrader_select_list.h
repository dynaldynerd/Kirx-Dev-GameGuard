#pragma once

#include "IdaCompat.h"
#pragma pack(push, 8)

struct _lt_qry_case_unmandtrader_select_list
{
  struct __list
  {
    unsigned int dwSerial;
    unsigned int dwK;
    unsigned __int64 dwD;
    unsigned int dwU;
    unsigned int dwPrice;
    unsigned int dwOwner;
    char wszOwnerName[17];
    __int64 tRegdate;
    unsigned __int8 bySellturm;
    unsigned int dwT;
    unsigned __int64 lnUID;
  };

  unsigned __int16 wInx;
  unsigned __int8 byPadding0[2];
  unsigned int dwOwnerSerial;
  unsigned __int8 byType;
  unsigned __int8 byRace;
  unsigned __int8 byPadding1[2];
  unsigned int dwListIndex;
  unsigned __int8 byClass1;
  unsigned __int8 byClass2;
  unsigned __int8 byClass3;
  unsigned __int8 bySortType;
  unsigned int dwVer;
  unsigned __int8 byPage;
  char szSortQuery[128];
  unsigned __int8 byProcRet;
  unsigned __int8 byPadding2[2];
  unsigned int dwMaxPage;
  unsigned int dwCnt;
  unsigned __int8 byPadding3[4];
  __list list[10];

  __int64 size() const
  {
    return sizeof(*this);
  }
};

static_assert(sizeof(_lt_qry_case_unmandtrader_select_list::__list) == 72);
static_assert(sizeof(_lt_qry_case_unmandtrader_select_list) == 0x378);

#pragma pack(pop)
