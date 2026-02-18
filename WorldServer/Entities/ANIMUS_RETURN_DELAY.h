#pragma once

#include "IdaCompat.h"
#include "DELAY_PROCESS.h"

struct  _ANIMUS_RETURN_DELAY : _DELAY_PROCESS
{
  _ANIMUS_RETURN_DELAY();
  virtual ~_ANIMUS_RETURN_DELAY();
  void Process(unsigned int dwIndex, unsigned int dwSerial) override;
};
