#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _enter_world_result_zone
{
  unsigned __int8 byResult;
  unsigned __int8 byUserGrade;
  unsigned __int8 bySvrType;

  int size() const;
};
#pragma pack(pop)

inline int _enter_world_result_zone::size() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(3LL);
}
