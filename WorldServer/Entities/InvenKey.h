#pragma once

#include "IdaCompat.h"

struct __cppobj _INVENKEY
{
  unsigned __int8 bySlotIndex;
  unsigned __int8 byTableCode;
  unsigned __int16 wItemIndex;

  _INVENKEY();
  _INVENKEY(unsigned __int8 byInSlotIndex, unsigned __int8 byInTableCode, unsigned __int16 wInItemIndex);

  _INVENKEY &operator=(const _INVENKEY &rhs);
  bool operator==(const _INVENKEY &rhs) const;

  void LoadDBKey(int pl_nKey);
  bool IsFilled();
  void SetRelease();
  __int64 CovDBKey();
};
