#include "pch.h"

#include "exchange_lend_item_result_zocl.h"

int _exchange_lend_item_result_zocl::size() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(21LL);
}
