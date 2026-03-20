#pragma once

#include "IdaCompat.h"

struct _qry_case_updatereservedschedule
{
  unsigned int dwMapID;
  unsigned int dwSLID;
  unsigned __int8 byLoadDataStartPosition;

  int size() const;
};

inline int _qry_case_updatereservedschedule::size() const
{
  return 12;
}

