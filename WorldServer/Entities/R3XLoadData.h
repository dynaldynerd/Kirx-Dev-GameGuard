#pragma once

#include "IdaCompat.h"

struct _R3X_LOAD_DATA
{
  int mFlag;
  int mValue4;
  __int64 mValue8;
  int mValue16;
  int mValue20;
  unsigned int mData[252];
};

static_assert(sizeof(_R3X_LOAD_DATA) == 1032);
