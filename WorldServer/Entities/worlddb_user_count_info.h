#pragma once

#include "IdaCompat.h"

struct _worlddb_user_count_info
{
  struct __user_count_info
  {
    unsigned __int16 wYear;
    unsigned __int8 byMonth;
    unsigned __int8 byDay;
    unsigned __int8 byHour;
    unsigned int dwAvgCount;
    unsigned int dwMaxCount;
  };

  unsigned __int16 wRowCount;
  __user_count_info UserCount[256];
};
