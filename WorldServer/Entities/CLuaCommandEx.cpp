#include "pch.h"

#include "CLuaCommandEx.h"

char *CLuaCommandEx::GetScriptName()
{
  return m_strScriptName;
}

void CLuaCommandEx::SetCmd(unsigned __int8 byType, const char *strScriptName, const char *strName)
{

  CLuaCommand::SetCmd(byType, strName);
  if (strScriptName)
  {
    strcpy_s(m_strScriptName, sizeof(m_strScriptName), strScriptName);
  }
}