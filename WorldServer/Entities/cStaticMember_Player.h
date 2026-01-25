#pragma once

#include "IdaCompat.h"

class __cppobj cStaticMember_Player
{
public:
  static cStaticMember_Player *Instance();
  bool Initialize();
};
