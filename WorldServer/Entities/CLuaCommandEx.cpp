#include "pch.h"

#include "CLuaCommandEx.h"

char *CLuaCommandEx::GetScriptName()
{
  return m_strScriptName;
}

void CLuaCommandEx::SetCmd(unsigned __int8 byType, const char *strScriptName, const char *strName)
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 8; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }

  CLuaCommand::SetCmd(byType, strName);
  if (strScriptName)
  {
    strcpy_s(m_strScriptName, sizeof(m_strScriptName), strScriptName);
  }
}
