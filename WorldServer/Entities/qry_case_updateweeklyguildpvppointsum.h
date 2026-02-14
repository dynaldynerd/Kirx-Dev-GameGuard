#pragma once

#include "IdaCompat.h"

struct _qry_case_updateweeklyguildpvppointsum
{
  unsigned int dwGuildSerial;
  long double dPvpPoint;

  __int64 size() const
  {
    return 16;
  }
};
