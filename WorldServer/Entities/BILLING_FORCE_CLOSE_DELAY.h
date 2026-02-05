#pragma once

#include "IdaCompat.h"
#include "DELAY_PROCESS.h"

struct __cppobj _BILLING_FORCE_CLOSE_DELAY : _DELAY_PROCESS
{
  _BILLING_FORCE_CLOSE_DELAY();
  virtual ~_BILLING_FORCE_CLOSE_DELAY();
  void Process(unsigned int dwIndex, unsigned int dwSerial) override;
};
