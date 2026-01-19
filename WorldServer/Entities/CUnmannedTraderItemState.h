#pragma once

#include "IdaCompat.h"

class __cppobj CUnmannedTraderItemState
{
public:
  enum class STATE : int
  {
    FIREEL = 0,
    WATEREL = 1,
    SOILEL = 2,
    WINDEL = 3,
    NONE = -1,
  };

  STATE m_eState;
};
