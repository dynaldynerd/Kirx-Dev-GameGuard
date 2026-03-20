#include "pch.h"

#include "chat_steal_message_gm_zocl.h"

_chat_steal_message_gm_zocl::_chat_steal_message_gm_zocl() : bySize(0) {}

int _chat_steal_message_gm_zocl::size() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(282LL - (255 - bySize));
}
