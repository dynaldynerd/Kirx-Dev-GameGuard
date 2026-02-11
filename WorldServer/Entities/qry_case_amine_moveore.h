#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(1)) _qry_case_amine_moveore
{
  unsigned __int8 bySubQryCase;
  unsigned __int8 byCollisionType;
  unsigned __int8 byRace;
  unsigned int dwGuildSerial;
  unsigned __int8 bySColmID;
  unsigned int dwSK;
  unsigned __int8 bySOverlapNum;
  unsigned __int8 byDColmID;
  unsigned int dwDK;
  unsigned __int8 byDOverlapNum;

  _qry_case_amine_moveore();
  __int64 size() const;
};

inline _qry_case_amine_moveore::_qry_case_amine_moveore()
  : bySubQryCase(6)
{
}

inline __int64 _qry_case_amine_moveore::size() const
{
  return 19;
}
