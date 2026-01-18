#pragma once

#include "IdaCompat.h"

/* 1675 */
struct __cppobj __declspec(align(8)) CLuaCommand
{
  unsigned __int8 m_byCommand;
  char m_strBuff[2048];
  virtual ~CLuaCommand() = default;
};
