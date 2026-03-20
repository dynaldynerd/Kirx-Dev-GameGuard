#pragma once

#include "IdaCompat.h"

struct _qry_case_addpvppoint
{
  unsigned int dwSerial;
  unsigned int dwPoint;
  unsigned int dwCashBag;

  int size() const
  {
    return 12;
  }
};
