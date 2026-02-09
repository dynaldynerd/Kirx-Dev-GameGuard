#pragma once

#include "IdaCompat.h"

struct _connection_status_result_zoct
{
  unsigned __int8 bStatus[6];

  unsigned __int16 size() const;
};

inline unsigned __int16 _connection_status_result_zoct::size() const
{
  return 6;
}
