#include "pch.h"

#include "trans_gm_msg_inform_zocl.h"

_trans_gm_msg_inform_zocl::_trans_gm_msg_inform_zocl()
{
  wMsgSize = 0;
}

int _trans_gm_msg_inform_zocl::size()
{
  if (wMsgSize > 1280)
  {
    wMsgSize = 0;
  }
  // narrowing cast for thunk return parity
  return static_cast<int>(1283LL - (1280 - wMsgSize));
}
