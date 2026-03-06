#pragma once

#include "IdaCompat.h"

struct _qry_case_unmandtrader_cheat_updateregisttime
{
  struct __list
  {
    unsigned __int8 byProcRet;
    unsigned __int8 byState;
    unsigned __int16 wPadding;
    unsigned int dwRegistSerial;
  };

  unsigned __int8 byType;
  unsigned __int8 byNum;
  unsigned __int16 wInx;
  unsigned int dwOwnerSerial;
  __list List[10];

  __int64 size()
  {
    if (byNum > 10)
    {
      byNum = 0;
    }

    return 88 - 8LL * (10 - byNum);
  }
};

static_assert(sizeof(_qry_case_unmandtrader_cheat_updateregisttime::__list) == 8);
static_assert(sizeof(_qry_case_unmandtrader_cheat_updateregisttime) == 88);
