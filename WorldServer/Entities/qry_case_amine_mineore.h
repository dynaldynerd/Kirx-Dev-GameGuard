#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(1)) _qry_case_amine_mineore
{
  unsigned __int8 bySubQryCase;
  unsigned __int8 byCollisionType;
  unsigned __int8 byRace;
  unsigned int dwGuildSerial;
  unsigned __int8 byColmID;
  unsigned int dwK;
  unsigned __int8 byOverlapNum;
  unsigned int dwGage;

  _qry_case_amine_mineore();
  __int64 size() const;
};

inline _qry_case_amine_mineore::_qry_case_amine_mineore()
  : bySubQryCase(2)
{
}

inline __int64 _qry_case_amine_mineore::size() const
{
  return 17;
}
