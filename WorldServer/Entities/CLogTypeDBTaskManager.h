#pragma once

#include "IdaCompat.h"

class __cppobj CLogTypeDBTaskManager
{
public:
  static CLogTypeDBTaskManager *Instance();
  bool Init();
};
