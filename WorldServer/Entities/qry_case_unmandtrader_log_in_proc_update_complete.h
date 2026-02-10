#pragma once

#include "IdaCompat.h"

struct __declspec(align(1)) _qry_case_unmandtrader_log_in_proc_update_complete
{
  struct __list
  {
    unsigned __int8 byProcRet;
    unsigned __int8 byOldState;
    unsigned __int16 wPadding;
    unsigned int dwRegistSerial;
  };

  unsigned __int8 byType;
  unsigned __int8 byNum;
  unsigned __int16 wInx;
  unsigned __int8 bAllSuccess;
  unsigned __int8 byPadding;
  unsigned __int16 wPadding2;
  unsigned int dwSeller;
  __list List[10];
};
