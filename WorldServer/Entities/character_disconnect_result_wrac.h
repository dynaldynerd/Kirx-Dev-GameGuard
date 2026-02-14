#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _character_disconnect_result_wrac
{
  unsigned __int16 wClientIndex;
  unsigned __int8 byResult;

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _character_disconnect_result_wrac::size() const
{
  return 3;
}
