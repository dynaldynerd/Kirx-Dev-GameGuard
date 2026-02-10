#include "pch.h"

#include "itembox_take_add_result_zocl.h"

__int64 _itembox_take_add_result_zocl::size() const
{
  if (sErrorCode)
  {
    return 1LL;
  }
  return 4LL;
}
