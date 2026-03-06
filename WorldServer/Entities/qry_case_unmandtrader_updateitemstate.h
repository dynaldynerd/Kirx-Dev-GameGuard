#pragma once

#include "IdaCompat.h"

struct _qry_case_unmandtrader_updateitemstate
{
  unsigned __int8 byType;
  unsigned int dwRegistSerial;
  unsigned __int8 byState;
  unsigned __int16 wItemSerial;
  unsigned int dwOwnerSerial;
  unsigned __int8 byItemTableCode;
  unsigned __int16 wItemTableIndex;

  __int64 size() const;
};

inline __int64 _qry_case_unmandtrader_updateitemstate::size() const
{
  return 20;
}

static_assert(sizeof(_qry_case_unmandtrader_updateitemstate) == 20);
