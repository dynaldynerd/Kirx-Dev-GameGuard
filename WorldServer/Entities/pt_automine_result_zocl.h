#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _pt_automine_result_zocl
{
  unsigned __int8 byType;
  unsigned __int8 byRetCode;

  __int64 size() const;
};
#pragma pack(pop)

inline __int64 _pt_automine_result_zocl::size() const
{
  return 2;
}
