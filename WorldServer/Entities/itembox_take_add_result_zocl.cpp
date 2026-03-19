#include "pch.h"

#include "itembox_take_add_result_zocl.h"

int _itembox_take_add_result_zocl::size() const
{
  if (sErrorCode)
  {
    // narrowing cast for thunk return parity
    return static_cast<int>(1LL);
  }
  // narrowing cast for thunk return parity
  return static_cast<int>(4LL);
}
