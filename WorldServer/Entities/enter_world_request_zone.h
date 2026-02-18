#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct  __declspec(align(2)) _enter_world_request_zone
{
  unsigned int dwAccountSerial;
  unsigned int dwMasterKey[4];
  bool bFullMode;
  unsigned int dwProtocolVer;
  char szClientVerCheckKey[33];
};
#pragma pack(pop)
