#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(1)) _moveout_user_result_zone
{
  unsigned __int8 byRetCode;
  unsigned int dwIP;
  unsigned __int16 wPort;
  unsigned int dwZoneMasterKey;

  unsigned __int16 size() const;
};

inline unsigned __int16 _moveout_user_result_zone::size() const
{
  return 11;
}
