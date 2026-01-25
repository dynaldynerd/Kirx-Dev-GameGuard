#pragma once

#include "IdaCompat.h"

class __cppobj CLuaScriptMgr
{
public:
  static CLuaScriptMgr *Instance();
  bool InitSDM();
};
