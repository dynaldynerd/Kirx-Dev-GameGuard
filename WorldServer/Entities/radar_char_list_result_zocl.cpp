#include "pch.h"

#include "radar_char_list_result_zocl.h"

_radar_char_list_result_zocl::_radar_char_list_result_zocl()
{
  bEnd = false;
  byListNum = 0;
}

__int64 _radar_char_list_result_zocl::size()
{
  if (byListNum > 0x32u)
  {
    byListNum = 0;
  }
  return 452 - 9LL * (50 - byListNum);
}
