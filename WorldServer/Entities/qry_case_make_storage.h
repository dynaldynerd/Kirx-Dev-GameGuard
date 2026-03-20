#pragma once

#include "IdaCompat.h"

struct  _qry_case_make_storage
{
  unsigned __int8 bySubQry;
  int nSockIdx;
  unsigned int dwAvatorSerial;

  _qry_case_make_storage();
  int size() const;
};

inline _qry_case_make_storage::_qry_case_make_storage()
  : bySubQry(0)
{
}

inline int _qry_case_make_storage::size() const
{
  return 12;
}
