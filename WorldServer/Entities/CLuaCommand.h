#pragma once

#include "IdaCompat.h"

/* 1675 */
class  __declspec(align(8)) CLuaCommand
{
public:
  unsigned __int8 m_byCommand;
  char m_strBuff[2048];
  virtual ~CLuaCommand() = default;
  void Init();
  unsigned __int8 GetType();
  char *GetBuffer();
  void SetCmd(unsigned __int8 byType, const char *strName);
};

