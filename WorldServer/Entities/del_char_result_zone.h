#pragma once

#include "IdaCompat.h"

struct _del_char_result_zone
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotIndex;

  __int64 size() const;
};

inline __int64 _del_char_result_zone::size() const
{
  return 2;
}
