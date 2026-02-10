#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(1)) _qry_case_amine_newowner
{
  unsigned __int8 bySubQryCase;
  unsigned __int8 byCollisionType;
  unsigned __int8 byRace;
  unsigned int dwGuildSerial;

  _qry_case_amine_newowner();
  __int64 size() const;
};

inline _qry_case_amine_newowner::_qry_case_amine_newowner()
  : bySubQryCase(0)
{
}

inline __int64 _qry_case_amine_newowner::size() const
{
  return 7;
}
