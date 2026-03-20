#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct  _moveout_user_result_zone
{
  unsigned __int8 byRetCode;
  unsigned int dwIP;
  unsigned __int16 wPort;
  unsigned int dwZoneMasterKey;

  int size() const;
};
#pragma pack(pop)

inline int _moveout_user_result_zone::size() const
{
  return 11;
}
