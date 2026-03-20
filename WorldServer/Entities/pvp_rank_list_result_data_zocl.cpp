#include "pch.h"

#include "pvp_rank_list_result_data_zocl.h"

_pvp_rank_list_result_data_zocl::_pvp_rank_list_result_data_zocl()
{
  wRankDataLen = 0;
}

int _pvp_rank_list_result_data_zocl::size()
{
  if (wRankDataLen >= 4000)
  {
    wRankDataLen = 0;
  }
  // narrowing cast for thunk return parity
  return static_cast<int>(5LL + wRankDataLen);
}
