#include "pch.h"

#include "CLuaScript.h"
#include "CLuaCommand.h"
#include "CLuaScriptMgr.h"
#include "CLogFile.h"

#include <cstdio>

namespace lua_tinker
{
  void dofile(lua_State *, const char *) {}
  void dostring(lua_State *, const char *) {}
}

CLuaScript::CLuaScript()
{
  m_MyState = nullptr;
  m_strName[0] = '\0';
}

const char *CLuaScript::GetName()
{
  return m_strName;
}

lua_State *CLuaScript::GetLuaState()
{
  return m_MyState;
}

bool CLuaScript::RunCommand(CLuaCommand *pCommand)
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 100; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }

  if (!pCommand)
  {
    return false;
  }

  if (pCommand->GetType() == 2)
  {
    char dest[260]{};
    const char *buffer = pCommand->GetBuffer();
    sprintf_s(dest, ".\\LuaScript\\%s.lua", buffer);
    CLogFile *stateLogFile = CLuaScriptMgr::Instance()->GetStateLogFile();
    stateLogFile->Write(" %s RunCommand File %s", GetName(), dest);
    lua_tinker::dofile(GetLuaState(), dest);
  }
  else if (pCommand->GetType() == 1)
  {
    char *buffer = pCommand->GetBuffer();
    CLogFile *stateLogFile = CLuaScriptMgr::Instance()->GetStateLogFile();
    stateLogFile->Write(" %s RunCommand string %s", GetName(), buffer);
    lua_tinker::dostring(GetLuaState(), buffer);
  }

  pCommand->Init();
  return true;
}

void CLuaScript::SetName(const char *name)
{
  if (name)
  {
    strcpy_s(m_strName, sizeof(m_strName), name);
  }
}
