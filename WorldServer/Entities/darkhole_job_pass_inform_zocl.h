#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _darkhole_job_pass_inform_zocl
{
  unsigned __int8 byJobIndex;

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _darkhole_job_pass_inform_zocl::size() const
{
  return 1;
}
