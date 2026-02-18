#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct  _server_notify_inform_zone
{
  unsigned __int16 wMsgCode;
  unsigned int dwPushIP;

  unsigned __int16 size() const;
};
#pragma pack(pop)
