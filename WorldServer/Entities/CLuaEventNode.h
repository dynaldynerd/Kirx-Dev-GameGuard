#pragma once

#include "IdaCompat.h"

class CLuaScript;

class __cppobj CLuaEventNode
{
public:
  class _State
  {
  public:
    bool m_bExist;
    bool m_bAttached;

    _State();
    bool operator==(const _State &rhs) const;

    static _State ms_cEmpty;
  };

  CLuaEventNode();
  ~CLuaEventNode();
  void Init();
  void SetName(const char *strName);
  void SetCallFun(const char *strCallFunName, unsigned long dwTime);
  void SetScript(CLuaScript *pParentScript);
  const char *GetName();
  char *GetCallFunName();
  unsigned int GetTimeTerm();
  unsigned int GetNextLoopTime();
  void SetNextLoopTime(unsigned int dwNextLoopTime);
  bool IsCallFun();
  CLuaScript *GetScript();

  char m_strName[64];
  bool m_bCallFunc;
  char m_strCallFuncName[64];
  unsigned int m_dwTimeTerm;
  unsigned int m_dwNextLoopTime;
  CLuaScript *m_pLuaScript;
};
