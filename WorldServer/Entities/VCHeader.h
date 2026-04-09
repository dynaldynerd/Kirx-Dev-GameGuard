#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct VCHeader
{
  unsigned __int16 encSize;
  unsigned __int8 paddingSize;

  void set(unsigned __int16 encSize_, unsigned __int8 paddingSize_);
};
#pragma pack(pop)

static_assert(sizeof(VCHeader) == 3);
