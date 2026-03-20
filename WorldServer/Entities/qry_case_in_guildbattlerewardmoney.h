#pragma once

#include "IdaCompat.h"

struct _qry_case_in_guildbattlerewardmoney
{
  unsigned int dwGuildIndex;
  unsigned int dwGuildSerial;
  unsigned int dwAddGold;
  unsigned int dwAddDalant;
  unsigned __int8 byDate[4];
  long double out_totalgold;
  long double out_totaldalant;

  int size() const
  {
    return 40;
  }
};
