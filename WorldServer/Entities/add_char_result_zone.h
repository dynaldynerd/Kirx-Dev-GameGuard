#pragma once

#include "IdaCompat.h"

struct _add_char_result_zone
{
  unsigned __int8 byRetCode;
  unsigned __int8 byAddSlotIndex;

  __int64 size() const;
};

inline __int64 _add_char_result_zone::size() const
{
  return 2;
}
