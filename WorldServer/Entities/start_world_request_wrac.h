#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(2)) _start_world_request_wrac
{
  unsigned int dwGateIP;
  unsigned __int16 wGatePort;

  unsigned __int16 size() const;
};

inline unsigned __int16 _start_world_request_wrac::size() const
{
  return 6;
}
