#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct  _start_world_request_wrac
{
  unsigned int dwGateIP;
  unsigned __int16 wGatePort;

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _start_world_request_wrac::size() const
{
  return 6;
}

