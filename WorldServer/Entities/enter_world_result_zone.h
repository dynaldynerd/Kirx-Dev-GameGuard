#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _enter_world_result_zone
{
  unsigned __int8 byResult;
  unsigned __int8 byUserGrade;
  unsigned __int8 bySvrType;

  __int64 size() const;
};
#pragma pack(pop)

inline __int64 _enter_world_result_zone::size() const
{
  return 3LL;
}
