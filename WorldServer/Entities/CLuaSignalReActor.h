#pragma once

#include "IdaCompat.h"

class CLuaCommandEx;

/* 1680 */
class __cppobj CLuaSignalReActor
{
  struct _Action
  {
    unsigned __int8 m_bySignalCode;
    CLuaCommandEx *m_pLuaCommandEx;

    _Action();
    void Init();
  };

  _Action m_ActionData[3];

  _Action *_SearchAction(unsigned __int8 bySignalCode);
  _Action *_SearchEmptyAction();
  void _Free(_Action *pAction);

public:
  CLuaSignalReActor();
  ~CLuaSignalReActor();
  void Init();
  void Free();
  bool SetSignalAndAction(unsigned __int8 bySignalCode);
  bool AddAction(unsigned __int8 bySignalCode, unsigned __int8 byType, const char *strScriptName, const char *strName);
};

