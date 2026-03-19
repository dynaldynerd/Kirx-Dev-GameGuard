#include "pch.h"
#include "guild_manage_request_clzo.h"

_guild_manage_request_clzo::_guild_manage_request_clzo()
{
  std::memset(this, 0, size());
}

int _guild_manage_request_clzo::size()
{
  // narrowing cast for thunk return parity
  return static_cast<int>(17LL);
}
