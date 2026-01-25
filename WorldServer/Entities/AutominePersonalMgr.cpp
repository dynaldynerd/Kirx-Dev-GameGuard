#include "pch.h"

#include "AutominePersonalMgr.h"

AutominePersonalMgr *AutominePersonalMgr::instance()
{
  static AutominePersonalMgr s_instance;
  return &s_instance;
}

bool AutominePersonalMgr::initialize()
{
  return true;
}
