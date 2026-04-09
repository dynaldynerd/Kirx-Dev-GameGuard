#pragma once

#include "IdaCompat.h"

struct _qry_case_unmandtrader_log_in_proc_update_complete
{
  struct __list
  {
    unsigned __int8 byProcRet;
    unsigned __int16 wItemSerial;
    unsigned __int8 bySellTurm;
    unsigned __int8 byProcUpdate;
    unsigned int dwBuyer;
    unsigned int dwRegistSerial;
    unsigned __int8 byUpdateState;
    unsigned int dwPrice;
    unsigned int dwTax;
    unsigned int dwAccountSerial;
    char szAccountID[21];
    char wszName[17];
    __int64 dbresulttime;
  };

  unsigned __int16 wInx;
  unsigned int dwSeller;
  bool bAllSuccess;
  unsigned __int8 byType;
  unsigned __int16 wNum;
  __list List[20];
};

using _qry_case_unmandtrader_log_in_proc_update_complete_create =
  _qry_case_unmandtrader_log_in_proc_update_complete;

static_assert(sizeof(_qry_case_unmandtrader_log_in_proc_update_complete::__list) == 80);
static_assert(sizeof(_qry_case_unmandtrader_log_in_proc_update_complete) == 1616);
