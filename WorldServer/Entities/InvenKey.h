#pragma once

#include "IdaCompat.h"

struct __cppobj _INVENKEY
{
  unsigned __int8 bySlotIndex;
  unsigned __int8 byTableCode;
  unsigned __int16 wItemIndex;

  bool IsFilled();
  void SetRelease();
};
