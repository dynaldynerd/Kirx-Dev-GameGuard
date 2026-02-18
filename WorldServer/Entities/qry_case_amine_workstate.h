#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct  _qry_case_amine_workstate
{
  unsigned __int8 bySubQryCase;
  unsigned __int8 byCollisionType;
  unsigned __int8 byRace;
   unsigned int dwGuildSerial;
  bool bWorking;

  _qry_case_amine_workstate();
  __int64 size() const;
};
#pragma pack(pop)

inline _qry_case_amine_workstate::_qry_case_amine_workstate()
  : bySubQryCase(3)
{
}

inline __int64 _qry_case_amine_workstate::size() const
{
  return 8;
}
