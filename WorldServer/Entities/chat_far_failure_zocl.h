#pragma once

#include "IdaCompat.h"

struct _chat_far_failure_zocl
{
  bool bBlock;

  unsigned __int16 size() const;
};

inline unsigned __int16 _chat_far_failure_zocl::size() const
{
  return static_cast<unsigned __int16>(sizeof(_chat_far_failure_zocl));
}
