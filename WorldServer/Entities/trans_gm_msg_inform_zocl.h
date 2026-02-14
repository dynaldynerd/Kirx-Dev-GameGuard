#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __cppobj __unaligned __declspec(align(1)) _trans_gm_msg_inform_zocl
{
  unsigned __int16 wMsgSize;
  char wszChatData[1281];

  _trans_gm_msg_inform_zocl();
  __int64 size();
};
#pragma pack(pop)
