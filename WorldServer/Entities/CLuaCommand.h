#pragma once

#include "IdaCompat.h"

/* 1675 */
class  CLuaCommand
{
public:
  CLuaCommand();
  virtual ~CLuaCommand();

  unsigned __int8 m_byCommand;
  char m_strBuff[2048];
  void Init();
  unsigned __int8 GetType();
  char *GetBuffer();
  void SetCmd(unsigned __int8 byType, const char *strName);
};


