#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _ccrfg_detect_alret
{
  unsigned __int8 byValue;

  _ccrfg_detect_alret() : byValue(0) {}

  unsigned __int16 size() const
  {
    return 1;
  }
};
#pragma pack(pop)
