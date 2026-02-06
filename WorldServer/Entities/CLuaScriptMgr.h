#pragma once

#include "IdaCompat.h"
#include <vector>
#include "CLogFile.h"

class CLuaScript;
class CLuaCommandEx;

class __cppobj CLuaScriptMgr
{
public:
  static CLuaScriptMgr *Instance();
  bool InitSDM();
  CLuaScript *SearchScript(const char *name);
  CLuaCommandEx *NewCommandEx();
  void RemoveCommandEx(CLuaCommandEx *cmd);
  CLogFile *GetStateLogFile();
  CLogFile *GetErrorLogFile();

private:
  CLuaScriptMgr() = default;
  ~CLuaScriptMgr() = default;

  CLuaScriptMgr(const CLuaScriptMgr &) = delete;
  CLuaScriptMgr &operator=(const CLuaScriptMgr &) = delete;

  std::vector<CLuaScript *> m_Scripts;
  std::vector<CLuaCommandEx *> m_CommandPool;
  CLogFile m_LogScriptError{};
  CLogFile m_LogScriptState{};
};
