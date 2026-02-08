#pragma once

#include "IdaCompat.h"
#include "DELAY_PROCESS.h"

struct __cppobj _MOVE_LOBBY_DELAY : _DELAY_PROCESS
{
  _MOVE_LOBBY_DELAY();
  virtual ~_MOVE_LOBBY_DELAY();
  void Process(unsigned int dwIndex, unsigned int dwSerial) override;
};
