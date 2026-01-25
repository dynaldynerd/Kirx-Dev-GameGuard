#include "pch.h"

#include "CLuaScriptMgr.h"

CLuaScriptMgr *CLuaScriptMgr::Instance()
{
  static CLuaScriptMgr s_instance;
  return &s_instance;
}

bool CLuaScriptMgr::InitSDM()
{
  return true;
}
