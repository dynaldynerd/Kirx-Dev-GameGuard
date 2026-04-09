#pragma once

#include "IdaCompat.h"

struct _unmannedtrader_result_buyerinfo
{
  unsigned int dwPrice;
  unsigned int dwTax;
  unsigned int dwBuyer;
  unsigned int dwAccountSerial;
  char szAccountID[21];
  char wszName[17];
  __int64 dbresulttime;
};

static_assert(sizeof(_unmannedtrader_result_buyerinfo) == 64);
