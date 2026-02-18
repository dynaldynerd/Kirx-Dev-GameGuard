#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct  _sel_char_result_zone
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotIndex;
  unsigned int dwWorldSerial;
  unsigned int dwDalant;
  unsigned int dwGold;

  __int64 size() const;
};
#pragma pack(pop)

inline __int64 _sel_char_result_zone::size() const
{
  return 14;
}
