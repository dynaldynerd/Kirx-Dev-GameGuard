#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct  _qry_case_amine_selore
{
  unsigned __int8 bySubQryCase;
  unsigned __int8 byCollisionType;
  unsigned __int8 byRace;
   unsigned int dwGuildSerial;
  unsigned __int8 byOreIdx;

  _qry_case_amine_selore();
  int size() const;
};
#pragma pack(pop)

inline _qry_case_amine_selore::_qry_case_amine_selore()
  : bySubQryCase(4)
{
}

inline int _qry_case_amine_selore::size() const
{
  return 8;
}
