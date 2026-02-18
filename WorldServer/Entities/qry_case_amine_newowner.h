#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct   _qry_case_amine_newowner
{
  unsigned __int8 bySubQryCase;
  unsigned __int8 byCollisionType;
  unsigned __int8 byRace;
  unsigned int dwGuildSerial;

  _qry_case_amine_newowner();
  __int64 size() const;
};
#pragma pack(pop)

inline _qry_case_amine_newowner::_qry_case_amine_newowner()
  : bySubQryCase(0)
{
}

inline __int64 _qry_case_amine_newowner::size() const
{
  return 7;
}
