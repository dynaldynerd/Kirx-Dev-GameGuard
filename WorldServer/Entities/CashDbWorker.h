#pragma once

#include "IdaCompat.h"

class __cppobj CashDbWorker
{
public:
  virtual ~CashDbWorker() = default;
  virtual bool Initialize() = 0;
};
