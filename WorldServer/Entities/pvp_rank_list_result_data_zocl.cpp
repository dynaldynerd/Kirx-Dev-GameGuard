#include "pch.h"

#include "pvp_rank_list_result_data_zocl.h"

_pvp_rank_list_result_data_zocl::_pvp_rank_list_result_data_zocl()
{
  wRankDataLen = 0;
}

__int64 _pvp_rank_list_result_data_zocl::size()
{
  if (wRankDataLen >= 0xFA0u)
  {
    wRankDataLen = 0;
  }
  return 5LL + wRankDataLen;
}
