#pragma once

#include "IdaCompat.h"

struct _qry_case_unmandtrader_time_out_cancelitem
{
  unsigned __int8 byType;
  unsigned int dwRegistSerial;
  unsigned __int8 byState;
  unsigned int dwOwnerSerial;
  unsigned __int8 byItemTableCode;
  unsigned __int16 wItemTableIndex;
  char szAccountID[13];
  char wszName[17];
  unsigned __int8 byProcRet;
  unsigned int dwK;
  unsigned int dwD;
  unsigned int dwU;
  unsigned int dwT;
  unsigned __int64 lnUID;
};
static_assert(sizeof(_qry_case_unmandtrader_time_out_cancelitem) == 80);
