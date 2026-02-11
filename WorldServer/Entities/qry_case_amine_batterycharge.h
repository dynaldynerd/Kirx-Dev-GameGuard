#pragma once

#include "IdaCompat.h"

#include <ctime>

struct __cppobj __unaligned __declspec(align(2)) _qry_case_amine_batterycharge
{
  unsigned __int8 bySubQryCase;
  unsigned __int8 byCollisionType;
  unsigned __int8 byRace;
  unsigned int dwGuildSerial;
  unsigned int dwBattery;
  unsigned __int16 wIndex;
  unsigned int in_master;
  int in_charge;
  int in_gold;
  long double out_totaldalant;
  long double out_totalgold;
  unsigned __int8 byDate[4];
  unsigned __int8 byProcRet;

  _qry_case_amine_batterycharge();
  __int64 size() const;
};

inline _qry_case_amine_batterycharge::_qry_case_amine_batterycharge()
  : bySubQryCase(1)
{
  std::time_t now = std::time(nullptr);
  std::tm *local = std::localtime(&now);
  if (local)
  {
    byDate[0] = static_cast<unsigned __int8>(local->tm_mon + 1);
    byDate[1] = static_cast<unsigned __int8>(local->tm_mday);
    byDate[2] = static_cast<unsigned __int8>(local->tm_hour);
    byDate[3] = static_cast<unsigned __int8>(local->tm_min);
  }
  else
  {
    byDate[0] = 0;
    byDate[1] = 0;
    byDate[2] = 0;
    byDate[3] = 0;
  }
}

inline __int64 _qry_case_amine_batterycharge::size() const
{
  return 46;
}
