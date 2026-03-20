#pragma once

#include "IdaCompat.h"

struct _qry_case_updateclearguildbattleDayInfo
{
  unsigned int dwStartSLID;
  unsigned int dwEndSLID;
  unsigned int dwStartSID;
  unsigned int dwEndSID;

  int size() const;
};

inline int _qry_case_updateclearguildbattleDayInfo::size() const
{
  return 16;
}

