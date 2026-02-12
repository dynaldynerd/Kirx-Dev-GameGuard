#pragma once

#include "IdaCompat.h"

struct _darkhole_answer_reenter_result_zocl
{
  unsigned __int8 byRetCode;

  unsigned __int16 size() const;
};

inline unsigned __int16 _darkhole_answer_reenter_result_zocl::size() const
{
  return 1;
}
