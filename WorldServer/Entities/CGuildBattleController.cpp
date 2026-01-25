#include "pch.h"

#include "CGuildBattleController.h"

CGuildBattleController *CGuildBattleController::Instance()
{
  static CGuildBattleController s_instance;
  return &s_instance;
}

bool CGuildBattleController::Init()
{
  return true;
}
