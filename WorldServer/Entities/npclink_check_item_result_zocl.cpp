#include "pch.h"

#include "npclink_check_item_result_zocl.h"

int _npclink_check_item_result_zocl::size() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(5LL);
}
