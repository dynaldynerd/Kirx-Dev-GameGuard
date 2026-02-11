#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(1)) _qry_case_amine_workstate
{
  unsigned __int8 bySubQryCase;
  unsigned __int8 byCollisionType;
  unsigned __int8 byRace;
  unsigned int dwGuildSerial;
  unsigned __int8 bWorking;

  _qry_case_amine_workstate();
  __int64 size() const;
};

inline _qry_case_amine_workstate::_qry_case_amine_workstate()
  : bySubQryCase(3)
{
}

inline __int64 _qry_case_amine_workstate::size() const
{
  return 8;
}
