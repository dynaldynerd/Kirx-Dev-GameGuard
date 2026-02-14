#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _chat_lock_inform_zocl
{
  bool bLock;

  __int64 size() const;
};
#pragma pack(pop)

inline __int64 _chat_lock_inform_zocl::size() const
{
  return 1;
}
