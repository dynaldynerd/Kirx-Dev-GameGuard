#include "pch.h"

#include "CLuaSignalReActor.h"
#include "CLuaScriptMgr.h"
#include "CLuaScript.h"

CLuaSignalReActor::_Action::_Action()
{
  Init();
}

void CLuaSignalReActor::_Action::Init()
{
  m_bySignalCode = static_cast<unsigned __int8>(-1);
  m_pLuaCommandEx = nullptr;
}

CLuaSignalReActor::CLuaSignalReActor()
{
  for (int j = 0; j < 3; ++j)
  {
    m_ActionData[j].Init();
  }
}

CLuaSignalReActor::~CLuaSignalReActor()
{
  // this is not a stub
}

void CLuaSignalReActor::Init()
{
  Free();
}

void CLuaSignalReActor::Free()
{
  for (int j = 0; j < 3; ++j)
  {
    _Free(&m_ActionData[j]);
  }
}

CLuaSignalReActor::_Action *CLuaSignalReActor::_SearchAction(unsigned __int8 bySignalCode)
{
  for (int j = 0; j < 3; ++j)
  {
    if (m_ActionData[j].m_bySignalCode == bySignalCode && m_ActionData[j].m_pLuaCommandEx)
    {
      return &m_ActionData[j];
    }
  }
  return nullptr;
}

CLuaSignalReActor::_Action *CLuaSignalReActor::_SearchEmptyAction()
{
  for (int j = 0; j < 3; ++j)
  {
    if (m_ActionData[j].m_bySignalCode == 0xFF && !m_ActionData[j].m_pLuaCommandEx)
    {
      return &m_ActionData[j];
    }
  }
  return nullptr;
}

void CLuaSignalReActor::_Free(_Action *pAction)
{
  if (pAction)
  {
    if (pAction->m_pLuaCommandEx)
    {
      CLuaScriptMgr::Instance()->RemoveCommandEx(pAction->m_pLuaCommandEx);
    }
    pAction->Init();
  }
}

bool CLuaSignalReActor::SetSignalAndAction(unsigned __int8 bySignalCode)
{

  bool result = false;
  _Action *action = _SearchAction(bySignalCode);
  if (!action)
  {
    return false;
  }

  char *scriptName = action->m_pLuaCommandEx->GetScriptName();
  CLuaScript *script = CLuaScriptMgr::Instance()->SearchScript(scriptName);
  if (script)
  {
    result = script->RunCommand(action->m_pLuaCommandEx);
  }
  _Free(action);
  return result;
}

bool CLuaSignalReActor::AddAction(
  unsigned __int8 bySignalCode,
  unsigned __int8 byType,
  const char *strScriptName,
  const char *strName)
{

  if (bySignalCode == 255)
  {
    return false;
  }

  _Action *action = _SearchEmptyAction();
  if (!action)
  {
    return false;
  }

  CLuaCommandEx *command = CLuaScriptMgr::Instance()->NewCommandEx();
  if (!command)
  {
    return false;
  }

  command->SetCmd(byType, strScriptName, strName);
  action->m_pLuaCommandEx = command;
  action->m_bySignalCode = bySignalCode;
  return true;
}
