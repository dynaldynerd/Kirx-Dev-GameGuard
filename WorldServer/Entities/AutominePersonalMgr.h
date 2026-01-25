#pragma once

#include "IdaCompat.h"

class __cppobj AutominePersonalMgr
{
public:
  static AutominePersonalMgr *instance();
  bool initialize();
};
