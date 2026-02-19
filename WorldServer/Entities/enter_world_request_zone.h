#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct  _enter_world_request_zone
{
  unsigned int dwAccountSerial;
  unsigned int dwMasterKey[4];
  bool bFullMode;
  unsigned int dwProtocolVer;
  char szClientVerCheckKey[33];
};
#pragma pack(pop)

