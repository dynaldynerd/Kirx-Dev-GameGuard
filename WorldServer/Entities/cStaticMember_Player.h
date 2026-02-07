#pragma once

#include "IdaCompat.h"

class __cppobj cStaticMember_Player
{
  int _nMaxLv;
  long double *_pLimExp;

  bool loadLimitExpData();

public:
  static cStaticMember_Player *Instance();
  bool Initialize();
};
