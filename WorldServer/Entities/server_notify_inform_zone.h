#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(2)) _server_notify_inform_zone
{
  unsigned __int16 wMsgCode;
  unsigned int dwPushIP;

  unsigned __int16 size() const;
};
