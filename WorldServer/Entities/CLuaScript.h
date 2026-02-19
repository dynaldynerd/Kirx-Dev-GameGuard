#pragma once

#include "IdaCompat.h"

class CLuaCommand;
struct lua_State;

class  CLuaScript
{
public:
  CLuaScript();
  virtual ~CLuaScript() = default;

  virtual const char *GetName();
  virtual lua_State *GetLuaState();
  virtual bool RunCommand(CLuaCommand *pCommand);
  virtual void SetName(const char *name);

  class _State
  {
  public:
    bool m_bExist;
    bool m_bAttached;

    _State();
    bool operator==(const _State &rhs) const;

    static _State ms_cEmpty;
  };

  lua_State *m_MyState;
  char m_strName[260];
};

