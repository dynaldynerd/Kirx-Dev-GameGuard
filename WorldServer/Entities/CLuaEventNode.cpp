#include "pch.h"

#include "CLuaEventNode.h"

#include "CLuaScript.h"
#include "WorldServerUtil.h"

CLuaEventNode::_State CLuaEventNode::_State::ms_cEmpty;

CLuaEventNode::_State::_State() : m_bExist(false), m_bAttached(false) {}

bool CLuaEventNode::_State::operator==(const _State &rhs) const
{
  return m_bExist == rhs.m_bExist && m_bAttached == rhs.m_bAttached;
}

CLuaEventNode::CLuaEventNode()
{
  m_strName[0] = '\0';
  m_bCallFunc = false;
  m_strCallFuncName[0] = '\0';
  m_dwTimeTerm = 0;
  m_dwNextLoopTime = 0;
  m_pLuaScript = nullptr;
}

CLuaEventNode::~CLuaEventNode()
{
  // this is not a stub
}

void CLuaEventNode::Init()
{
  m_bCallFunc = false;
  m_pLuaScript = nullptr;
}

void CLuaEventNode::SetName(const char *strName)
{
  strcpy_s(m_strName, sizeof(m_strName), strName);
}

void CLuaEventNode::SetCallFun(const char *strCallFunName, unsigned long dwTime)
{
  if (!strCallFunName)
  {
    m_bCallFunc = false;
    return;
  }

  unsigned int termTime = static_cast<unsigned int>(dwTime);
  if (dwTime < 5000u)
  {
    termTime = 5000;
  }

  strcpy_s(m_strCallFuncName, sizeof(m_strCallFuncName), strCallFunName);
  m_dwTimeTerm = termTime;
  m_dwNextLoopTime = m_dwTimeTerm + GetLoopTime();
  m_bCallFunc = true;
}

void CLuaEventNode::SetScript(CLuaScript *pParentScript)
{
  m_pLuaScript = pParentScript;
}

const char *CLuaEventNode::GetName()
{
  return m_strName;
}

char *CLuaEventNode::GetCallFunName()
{
  if (m_bCallFunc)
  {
    return m_strCallFuncName;
  }
  return nullptr;
}

unsigned int CLuaEventNode::GetTimeTerm()
{
  return m_dwTimeTerm;
}

unsigned int CLuaEventNode::GetNextLoopTime()
{
  return m_dwNextLoopTime;
}

void CLuaEventNode::SetNextLoopTime(unsigned int dwNextLoopTime)
{
  m_dwNextLoopTime = dwNextLoopTime;
}

bool CLuaEventNode::IsCallFun()
{
  return m_bCallFunc;
}

CLuaScript *CLuaEventNode::GetScript()
{
  return m_pLuaScript;
}
