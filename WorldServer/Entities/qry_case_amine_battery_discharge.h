#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(1)) _qry_case_amine_battery_discharge
{
  unsigned __int8 bySubQryCase;
  unsigned __int8 byCollisionType;
  unsigned __int8 byRace;
  unsigned int dwGuildSerial;
  unsigned int dwBattery;

  _qry_case_amine_battery_discharge();
  __int64 size() const;
};

inline _qry_case_amine_battery_discharge::_qry_case_amine_battery_discharge()
  : bySubQryCase(5)
{
}

inline __int64 _qry_case_amine_battery_discharge::size() const
{
  return 11;
}
