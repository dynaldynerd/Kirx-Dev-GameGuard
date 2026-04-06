#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _AHNHS_TRANS_BUFFER
{
  char byBuffer[400];
  unsigned __int16 nLength;
};
static_assert(sizeof(_AHNHS_TRANS_BUFFER) == 402);
#pragma pack(pop)
