#pragma once

#include "IdaCompat.h"

struct _chat_lock_inform_zocl
{
  bool bLock;

  __int64 size() const;
};

inline __int64 _chat_lock_inform_zocl::size() const
{
  return 1;
}
