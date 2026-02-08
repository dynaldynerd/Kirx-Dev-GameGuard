#include "pch.h"

#include "CHEAT_COMMAND.h"

bool CHEAT_COMMAND::operator()(const CHEAT_COMMAND *lhs, const CHEAT_COMMAND *rhs)
{
  return lhs->uiCmdLen > rhs->uiCmdLen;
}
