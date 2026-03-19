#pragma once

#include "IdaCompat.h"

struct _qry_case_update_pvp_info
{
  unsigned int dwSerial;
  unsigned __int8 byGrade;
  unsigned __int8 byPadding;
  __int16 zClassHistory[3];
  unsigned int dwPadding;
  long double dPvpPoint;

  int size() const
  {
    return sizeof(*this);
  }
};

static_assert(sizeof(_qry_case_update_pvp_info) == 24);
