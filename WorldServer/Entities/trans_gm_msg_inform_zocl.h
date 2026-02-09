#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(1)) _trans_gm_msg_inform_zocl
{
  unsigned __int16 wMsgSize;
  char wszChatData[1281];

  _trans_gm_msg_inform_zocl();
  unsigned __int16 size();
};

inline _trans_gm_msg_inform_zocl::_trans_gm_msg_inform_zocl()
  : wMsgSize(0)
{
}

inline unsigned __int16 _trans_gm_msg_inform_zocl::size()
{
  if (wMsgSize > 0x500u)
  {
    wMsgSize = 0;
  }
  return static_cast<unsigned __int16>(3 + wMsgSize);
}
