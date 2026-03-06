#pragma once

#include "IdaCompat.h"

struct _qry_case_unmandtrader_re_registsingleitem
{
  struct __list
  {
    unsigned __int8 byProcRet;
    bool bRegist;
    unsigned __int16 wItemSerial;
    unsigned int dwTax;
    unsigned int dwListIndex;
    unsigned __int8 byClass1;
    unsigned __int8 byClass2;
    unsigned __int8 byClass3;
    unsigned __int8 byPadding0;
    unsigned int dwPrice;
    unsigned int dwRegistSerial;
    unsigned __int8 byUpdateState;
    unsigned __int8 byPadding1[3];
  };

  unsigned __int8 byType;
  unsigned __int8 byPadding0;
  unsigned __int16 wInx;
  unsigned __int8 byNum;
  unsigned __int8 byPadding1[3];
  unsigned int dwOwnerSerial;
  __list List[10];

  __int64 size()
  {
    if (byNum > 10)
    {
      byNum = 0;
    }

    return 292 - 28LL * (10 - byNum);
  }
};

static_assert(sizeof(_qry_case_unmandtrader_re_registsingleitem::__list) == 28);
static_assert(sizeof(_qry_case_unmandtrader_re_registsingleitem) == 292);
