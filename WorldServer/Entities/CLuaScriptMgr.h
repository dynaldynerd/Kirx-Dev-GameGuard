#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CLuaCommandEx.h"
#include "CLuaScript.h"
#include "USArray.h"

class CLuaCommand;
struct lua_State;

class  CLuaScriptMgr
{
public:
  static CLuaScriptMgr *Instance();
  static void Destroy();

  bool InitSDM();

  CLuaScript *SearchScript(const char *name);
  CLuaScript *SearchScriptFromLuaState(lua_State *state);
  CLuaScript *NewScript();
  CLuaCommandEx *NewCommandEx();
  void RemoveCommandEx(CLuaCommandEx *cmd);
  void RemoveScript(CLuaScript *pScript);
  bool AttachLuaScript(CLuaScript *pScript, CLuaCommand *pAttachCommand);
  bool DetackLuaScript(CLuaScript *pScript);
  void LogStack(CLuaScript *pScript);
  void Loop();

  CLogFile *GetStateLogFile();
  CLogFile *GetErrorLogFile();

private:
  CLuaScriptMgr();
  ~CLuaScriptMgr();

  bool _Regist_Novus();

  CLuaScriptMgr(const CLuaScriptMgr &) = delete;
  CLuaScriptMgr &operator=(const CLuaScriptMgr &) = delete;

  static CLuaScriptMgr *ms_Instance;

  lua_State *m_MasterState;
  US::CArrayEx<CLuaScript, CLuaScript::_State> m_ChildScriptArEx;
  US::CArrayEx<CLuaCommandEx, CLuaCommandEx::_State> m_ExternCommandArEx;
  CLogFile m_LogScriptError;
  CLogFile m_LogScriptState;
};
