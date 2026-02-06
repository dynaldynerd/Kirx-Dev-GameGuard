#pragma once

#include "IdaCompat.h"

struct __cppobj _INVENKEY
{
  unsigned __int8 bySlotIndex;
  unsigned __int8 byTableCode;
  unsigned __int16 wItemIndex;

  _INVENKEY();
  _INVENKEY(unsigned __int8 byInSlotIndex, unsigned __int8 byInTableCode, unsigned __int16 wInItemIndex);

  bool IsFilled();
  void SetRelease();
  int CovDBKey();
};
