#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(1)) _qry_case_make_storage
{
  unsigned __int8 bySubQry;
  unsigned __int8 pad[3];
  int nSockIdx;
  unsigned int dwAvatorSerial;

  _qry_case_make_storage();
  __int64 size() const;
};

inline _qry_case_make_storage::_qry_case_make_storage()
  : bySubQry(0)
{
  pad[0] = 0;
  pad[1] = 0;
  pad[2] = 0;
}

inline __int64 _qry_case_make_storage::size() const
{
  return 12;
}
