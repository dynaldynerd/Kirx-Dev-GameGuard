#pragma once

#include "IdaCompat.h"

struct _qry_case_unmandtrader_log_in_proc_update_complete
{
  struct __list
  {
    unsigned __int8 byProcRet;
    unsigned __int8 byProcUpdate;
    unsigned __int16 wPadding;
    unsigned int dwBuyer;
    unsigned int dwRegistSerial;
    unsigned __int8 byUpdateState;
    unsigned __int8 byPadding[3];
  };

  unsigned __int16 wInx;
  unsigned __int16 wPadding0;
  unsigned int dwSeller;
  bool bAllSuccess;
  unsigned __int8 byType;
  unsigned __int16 wNum;
  __list List[20];
};

using _qry_case_unmandtrader_log_in_proc_update_complete_create =
  _qry_case_unmandtrader_log_in_proc_update_complete;

static_assert(sizeof(_qry_case_unmandtrader_log_in_proc_update_complete::__list) == 16);
static_assert(sizeof(_qry_case_unmandtrader_log_in_proc_update_complete) == 332);
