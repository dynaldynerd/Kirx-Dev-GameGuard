#include "pch.h"

#include "radar_char_list_result_zocl.h"

_radar_char_list_result_zocl::_radar_char_list_result_zocl()
{
  bEnd = false;
  byListNum = 0;
}

int _radar_char_list_result_zocl::size()
{
  if (byListNum > 50)
  {
    byListNum = 0;
  }
  // narrowing cast for thunk return parity
  return static_cast<int>(452 - 9LL * (50 - byListNum));
}
