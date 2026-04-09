#pragma once

#include "IdaCompat.h"

struct _qry_case_unmandtrader_update_reprice
{
  unsigned int dwOldPrice;
  unsigned __int16 wInx;
  unsigned int dwOwnerSerial;
  unsigned __int16 wItemSerial;
  unsigned int dwTax;
  unsigned __int8 byProcRet;
  unsigned __int8 byType;
  unsigned int dwRegistSerial;
  unsigned int dwNewPrice;

  int size() const
  {
    return sizeof(*this);
  }
};

static_assert(sizeof(_qry_case_unmandtrader_update_reprice) == 32);
