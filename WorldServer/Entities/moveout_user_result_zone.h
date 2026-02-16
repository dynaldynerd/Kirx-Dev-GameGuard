#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned _moveout_user_result_zone
{
  unsigned __int8 byRetCode;
  unsigned int dwIP;
  unsigned __int16 wPort;
  unsigned int dwZoneMasterKey;

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _moveout_user_result_zone::size() const
{
  return 11;
}
