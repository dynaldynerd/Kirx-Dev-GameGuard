#pragma once

#include "IdaCompat.h"

class CPlayer;

class __cppobj CHEAT_COMMAND
{
public:
  bool operator()(const CHEAT_COMMAND *lhs, const CHEAT_COMMAND *rhs);

  const char *pwszCommand;
  unsigned int uiCmdLen;
  bool (__fastcall *fn)(CPlayer *);
  int nUseDegree;
  int nMgrDegree;
};
