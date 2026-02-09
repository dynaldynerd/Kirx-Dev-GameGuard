#pragma once

#include "IdaCompat.h"

struct _chat_lock_inform_zocl
{
  unsigned __int8 bLock;

  unsigned __int16 size() const;
};

inline unsigned __int16 _chat_lock_inform_zocl::size() const
{
  return 1;
}
