#pragma once

#include "IdaCompat.h"

struct __cppobj _qry_case_amine_selore
{
  unsigned __int8 bySubQryCase;
  unsigned __int8 byCollisionType;
  unsigned __int8 byRace;
  __unaligned __declspec(align(1)) unsigned int dwGuildSerial;
  unsigned __int8 byOreIdx;

  _qry_case_amine_selore();
  __int64 size() const;
};

inline _qry_case_amine_selore::_qry_case_amine_selore()
  : bySubQryCase(4)
{
}

inline __int64 _qry_case_amine_selore::size() const
{
  return 8;
}
