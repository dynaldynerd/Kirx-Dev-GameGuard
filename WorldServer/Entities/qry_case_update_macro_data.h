#pragma once

#include "CMainThread.h"

struct _qry_case_update_macro_data
{
  unsigned int dwSerial;
  _AIOC_A_MACRODATA NewData;
  _AIOC_A_MACRODATA OldData;

  __int64 size() const
  {
    return sizeof(*this);
  }
};

static_assert(sizeof(_qry_case_update_macro_data) == 5412);
