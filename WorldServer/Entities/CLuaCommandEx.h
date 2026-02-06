#pragma once

#include "IdaCompat.h"

/* 1677 */
class __cppobj __declspec(align(8)) CLuaCommandEx : public CLuaCommand
{
  char m_strScriptName[260];
public:
  char *GetScriptName();
  void SetCmd(unsigned __int8 byType, const char *strScriptName, const char *strName);
};

