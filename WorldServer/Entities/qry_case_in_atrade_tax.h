#pragma once

#include "IdaCompat.h"
#include "WorldServerUtil.h"

struct  __declspec(align(8)) _qry_case_in_atrade_tax
{
  unsigned __int8 byRace;
  unsigned int dwGuildSerial;
  unsigned int in_seller;
  unsigned int in_dalant;
  long double out_totaldalant;
  long double out_totalgold;
  unsigned __int8 byDate[4];

  _qry_case_in_atrade_tax();
  __int64 size() const;
};

inline _qry_case_in_atrade_tax::_qry_case_in_atrade_tax()
{
  byDate[0] = GetCurrentMonth();
  byDate[1] = GetCurrentDay();
  byDate[2] = GetCurrentHour();
  byDate[3] = GetCurrentMin();
}

inline __int64 _qry_case_in_atrade_tax::size() const
{
  return 40;
}
