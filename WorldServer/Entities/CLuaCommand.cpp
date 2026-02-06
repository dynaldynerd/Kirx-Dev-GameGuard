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
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 8; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }

  m_byCommand = byType;
  if (strName)
  {
    strcpy_s(m_strBuff, sizeof(m_strBuff), strName);
  }
}
