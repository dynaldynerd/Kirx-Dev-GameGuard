#pragma once

#include "IdaCompat.h"

class CPlayer;

class __cppobj CHEAT_COMMAND
{
public:
  virtual ~CHEAT_COMMAND() = default;

  const char *pwszCommand;
  unsigned int uiCmdLen;
  bool (__fastcall *fn)(CPlayer *);
  int nUseDegree;
  int nMgrDegree;
};
