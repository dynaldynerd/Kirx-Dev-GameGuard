#pragma once

#include "IdaCompat.h"

class __cppobj CActionPointSystemMgr
{
public:
  static CActionPointSystemMgr *Instance();
  bool Initialize();
};

