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
  unsigned int dwOwnerSerial;
  char byType;
  char byRace;
  unsigned int dwListIndex;
  char byClass1;
  char byClass2;
  char byClass3;
  char bySortType;
  unsigned int dwVer;
  char byPage;
  char szSortQuery[128];
  char byProcRet;
  unsigned int dwMaxPage;
  unsigned int dwCnt;
  __list list[10];

  __int64 size() const
  {
    return sizeof(*this);
  }
};

static_assert(sizeof(_lt_qry_case_unmandtrader_select_list::__list) == 72);
static_assert(sizeof(_lt_qry_case_unmandtrader_select_list) == 888);

#pragma pack(pop)
