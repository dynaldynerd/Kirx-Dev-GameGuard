#include "pch.h"

#include "CLuaScriptMgr.h"
#include "CLuaScript.h"
#include "CLuaCommandEx.h"

#include <algorithm>

CLuaScriptMgr *CLuaScriptMgr::Instance()
{
  static CLuaScriptMgr s_instance;
  return &s_instance;
}

bool CLuaScriptMgr::InitSDM()
{
  return true;
}

CLuaScript *CLuaScriptMgr::SearchScript(const char *name)
{
  if (!name)
  {
    return nullptr;
  }
  for (CLuaScript *script : m_Scripts)
  {
    if (script && strcmp_0(script->GetName(), name) == 0)
    {
      return script;
    }
  }
  return nullptr;
}

CLuaCommandEx *CLuaScriptMgr::NewCommandEx()
{
  CLuaCommandEx *cmd = new CLuaCommandEx();
  if (cmd)
  {
    m_CommandPool.push_back(cmd);
  }
  return cmd;
}

void CLuaScriptMgr::RemoveCommandEx(CLuaCommandEx *cmd)
{
  if (!cmd)
  {
    return;
  }
  auto it = std::find(m_CommandPool.begin(), m_CommandPool.end(), cmd);
  if (it != m_CommandPool.end())
  {
    delete *it;
    m_CommandPool.erase(it);
  }
  else
  {
    delete cmd;
  }
}

CLogFile *CLuaScriptMgr::GetStateLogFile()
{
  return &m_LogScriptState;
}

CLogFile *CLuaScriptMgr::GetErrorLogFile()
{
  return &m_LogScriptError;
}
