#include "pch.h"

#include "CLuaScript.h"
#include "CLuaCommand.h"
#include "CLuaScriptMgr.h"
#include "CLogFile.h"
#include "LuaTinker.h"

#include <cstdio>

CLuaScript::_State CLuaScript::_State::ms_cEmpty{};

CLuaScript::_State::_State() : m_bExist(false), m_bAttached(false) {}

bool CLuaScript::_State::operator==(const _State &rhs) const
{
  return m_bExist == rhs.m_bExist && m_bAttached == rhs.m_bAttached;
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
