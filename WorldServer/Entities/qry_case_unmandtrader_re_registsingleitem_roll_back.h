#pragma once

#include "IdaCompat.h"

struct _qry_case_unmandtrader_re_registsingleitem_roll_back
{
  struct __list
  {
    unsigned __int8 byProcRet;
    unsigned __int8 byState;
    unsigned int dwRegistSerial;
  };

  unsigned __int8 byType;
  unsigned __int8 byNum;
  unsigned __int16 wInx;
  bool bAllSuccess;
  unsigned int dwOwnerSerial;
  __list List[10];
};

static_assert(sizeof(_qry_case_unmandtrader_re_registsingleitem_roll_back::__list) == 8);
static_assert(sizeof(_qry_case_unmandtrader_re_registsingleitem_roll_back) == 92);
