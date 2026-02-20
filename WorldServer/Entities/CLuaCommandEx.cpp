#include "pch.h"

#include "CLuaCommandEx.h"

CLuaCommandEx::_State CLuaCommandEx::_State::ms_cEmpty;

CLuaCommandEx::~CLuaCommandEx() = default;

CLuaCommandEx::_State::_State() : m_bExist(false) {}

bool CLuaCommandEx::_State::operator==(const _State &rhs) const
{
  return m_bExist == rhs.m_bExist;
}

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
