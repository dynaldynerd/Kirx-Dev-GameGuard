#include "pch.h"

#include "guildroom_out_result_zocl.h"

int _guildroom_out_result_zocl::size()
{
  // narrowing cast for thunk return parity
  return static_cast<int>(10LL);
}
