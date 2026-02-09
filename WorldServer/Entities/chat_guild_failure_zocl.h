#pragma once

#include "IdaCompat.h"

struct _chat_guild_failure_zocl
{
  unsigned __int8 byErrCode;

  unsigned __int16 size() const;
};

inline unsigned __int16 _chat_guild_failure_zocl::size() const
{
  return static_cast<unsigned __int16>(sizeof(_chat_guild_failure_zocl));
}
