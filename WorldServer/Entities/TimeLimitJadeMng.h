#pragma once

#include "IdaCompat.h"

class __cppobj TimeLimitJadeMng
{
public:
  static TimeLimitJadeMng *Instance();
  bool Init();
};
