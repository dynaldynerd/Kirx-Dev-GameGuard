#include "pch.h"

#include "CLuaCommand.h"

void CLuaCommand::Init()
{
  m_byCommand = 0;
}

unsigned __int8 CLuaCommand::GetType()
{
  return m_byCommand;
}

char *CLuaCommand::GetBuffer()
{
  return m_strBuff;
}

void CLuaCommand::SetCmd(unsigned __int8 byType, const char *strName)
{

  m_byCommand = byType;
  if (strName)
  {
    strcpy_s(m_strBuff, sizeof(m_strBuff), strName);
  }
}