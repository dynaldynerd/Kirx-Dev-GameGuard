#pragma once

#include "IdaCompat.h"

struct __declspec(align(4)) _qry_case_updateclearguildbattleDayInfo
{
  unsigned int dwStartSLID;
  unsigned int dwEndSLID;
  unsigned int dwStartSID;
  unsigned int dwEndSID;

  __int64 size() const;
};

inline __int64 _qry_case_updateclearguildbattleDayInfo::size() const
{
  return 16;
}
