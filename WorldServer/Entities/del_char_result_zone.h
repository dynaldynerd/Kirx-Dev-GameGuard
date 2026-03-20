#pragma once

#include "IdaCompat.h"

struct _del_char_result_zone
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotIndex;

  int size() const;
};

inline int _del_char_result_zone::size() const
{
  return 2;
}
