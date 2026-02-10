#include "pch.h"

#include "chat_steal_message_gm_zocl.h"

_chat_steal_message_gm_zocl::_chat_steal_message_gm_zocl() : bySize(0) {}

__int64 _chat_steal_message_gm_zocl::size() const
{
  return 282LL - (255 - bySize);
}
