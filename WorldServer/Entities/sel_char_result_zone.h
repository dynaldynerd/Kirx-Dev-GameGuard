#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(2)) _sel_char_result_zone
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotIndex;
  unsigned int dwWorldSerial;
  unsigned int dwDalant;
  unsigned int dwGold;

  __int64 size() const;
};

inline __int64 _sel_char_result_zone::size() const
{
  return 14;
}
