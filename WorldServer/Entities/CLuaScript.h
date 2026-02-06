#pragma once

#include "IdaCompat.h"

class CLuaCommand;
struct lua_State;

class __cppobj __declspec(align(8)) CLuaScript
{
public:
  CLuaScript();
  ~CLuaScript() = default;

  const char *GetName();
  lua_State *GetLuaState();
  bool RunCommand(CLuaCommand *pCommand);
  void SetName(const char *name);

private:
  lua_State *m_MyState;
  char m_strName[260];
};
