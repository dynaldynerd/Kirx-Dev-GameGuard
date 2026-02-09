#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _del_char_result_zone
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySlotIndex;

  __int64 size() const;
};
#pragma pack(pop)

inline __int64 _del_char_result_zone::size() const
{
  return 2LL;
}
