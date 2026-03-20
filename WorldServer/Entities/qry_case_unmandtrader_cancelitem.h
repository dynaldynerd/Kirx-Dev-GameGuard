#pragma once

#include "IdaCompat.h"

struct _qry_case_unmandtrader_cancelitem
{
  unsigned __int16 wInx;
  unsigned __int16 wItemSerial;
  unsigned int dwOwnerSerial;
  char szAccount[13];
  char wszName[17];
  unsigned __int8 byType;
  unsigned __int8 byPadding0;
  unsigned int dwRegistSerial;
  unsigned __int8 byState;
  unsigned __int8 byProcRet;
  unsigned __int8 byPadding1[2];
  unsigned int dwK;
  unsigned int dwD;
  unsigned int dwU;

  int size() const
  {
    return sizeof(*this);
  }
};

static_assert(sizeof(_qry_case_unmandtrader_cancelitem) == 60);
