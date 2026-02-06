#include "pch.h"

#include "CLuaSignalReActor.h"
#include "CLuaScriptMgr.h"
#include "CLuaScript.h"

CLuaSignalReActor::_Action::_Action()
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 8; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }
  Init();
}

void CLuaSignalReActor::_Action::Init()
{
  m_bySignalCode = static_cast<unsigned __int8>(-1);
  m_pLuaCommandEx = nullptr;
}

CLuaSignalReActor::CLuaSignalReActor()
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 8; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }
  for (int j = 0; j < 3; ++j)
  {
    m_ActionData[j].Init();
  }
}

void CLuaSignalReActor::Init()
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 8; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }
  Free();
}

void CLuaSignalReActor::Free()
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 12; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }
  for (int j = 0; j < 3; ++j)
  {
    _Free(&m_ActionData[j]);
  }
}

CLuaSignalReActor::_Action *CLuaSignalReActor::_SearchAction(unsigned __int8 bySignalCode)
{
  int stackFill = 0;
  int *fillPtr = &stackFill;
  for (int fillCount = 4; fillCount; --fillCount)
  {
    *fillPtr++ = -858993460;
  }
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
  int stackFill = 0;
  int *fillPtr = &stackFill;
  for (int fillCount = 4; fillCount; --fillCount)
  {
    *fillPtr++ = -858993460;
  }
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
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 8; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }
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
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 16; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }

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
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 12; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }

  if (bySignalCode == 0xFF)
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

