#pragma once

#include "IdaCompat.h"

struct CLuaCommandEx;

/* 1680 */
struct __cppobj CLuaSignalReActor
{
  struct _Action
  {
    unsigned __int8 m_bySignalCode;
    CLuaCommandEx *m_pLuaCommandEx;
  };

  _Action m_ActionData[3];
};
