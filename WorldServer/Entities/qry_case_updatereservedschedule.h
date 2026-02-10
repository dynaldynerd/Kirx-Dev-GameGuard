#pragma once

#include "IdaCompat.h"

struct __declspec(align(4)) _qry_case_updatereservedschedule
{
  unsigned int dwMapID;
  unsigned int dwSLID;
  unsigned __int8 byLoadDataStartPosition;

  __int64 size() const;
};

inline __int64 _qry_case_updatereservedschedule::size() const
{
  return 12;
}
