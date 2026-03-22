#pragma once

#include "IdaCompat.h"

struct  _qry_case_select_timelimit_info
{
  unsigned __int16 wIndex;
  unsigned int dwAccountSerial;
  unsigned __int64 dwLastLogoutTime;

  int size() const;
};

inline int _qry_case_select_timelimit_info::size() const
{
  return sizeof(*this);
}
