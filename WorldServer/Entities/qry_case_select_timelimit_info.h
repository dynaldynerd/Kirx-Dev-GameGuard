#pragma once

#include "IdaCompat.h"

struct  _qry_case_select_timelimit_info
{
  unsigned __int16 wIndex;
  unsigned int dwAccountSerial;
  unsigned int dwLastLogoutTime;

  __int64 size() const;
};

inline __int64 _qry_case_select_timelimit_info::size() const
{
  return 12;
}
