#pragma once

#include "IdaCompat.h"

class __cppobj CashItemRemoteStore
{
public:
  static CashItemRemoteStore *Instance();
  bool Initialize();
};
