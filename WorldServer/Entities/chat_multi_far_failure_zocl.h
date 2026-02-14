#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _chat_multi_far_failure_zocl
{
  char wszDstName[17];
  unsigned __int8 byFailCode;

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _chat_multi_far_failure_zocl::size() const
{
  return static_cast<unsigned __int16>(sizeof(_chat_multi_far_failure_zocl));
}
