#pragma once

#include "IdaCompat.h"

class __cppobj CGuildBattleController
{
public:
  static CGuildBattleController *Instance();
  bool Init();

  virtual ~CGuildBattleController() = default;
};

