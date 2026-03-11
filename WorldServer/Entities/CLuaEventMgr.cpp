#include "pch.h"

#include "CLuaEventMgr.h"

#include "CLuaScript.h"
#include "LuaTinker.h"
#include "WorldServerUtil.h"

#include <new>

CLuaEventMgr *CLuaEventMgr::ms_Instance;

CLuaEventMgr::CLuaEventMgr()
{
  m_LuaEventArEx.Alloc(20u);
}

CLuaEventMgr::~CLuaEventMgr() = default;

CLuaEventMgr *CLuaEventMgr::Instance()
{
  if (!CLuaEventMgr::ms_Instance)
  {
    CLuaEventMgr::ms_Instance = new (std::nothrow) CLuaEventMgr();
  }
  return CLuaEventMgr::ms_Instance;
}

void CLuaEventMgr::Destroy()
{
  if (CLuaEventMgr::ms_Instance)
  {
    delete CLuaEventMgr::ms_Instance;
    CLuaEventMgr::ms_Instance = nullptr;
  }
}

bool CLuaEventMgr::InitSDM()
{
  // this is not a stub
  return true;
}

void CLuaEventMgr::Loop()
{
  const unsigned int size = m_LuaEventArEx.GetSize();
  for (unsigned int index = 0; index < size; ++index)
  {
    CLuaEventNode::_State *state = m_LuaEventArEx.GetStateAtPtr(index);
    if (!state || !state->m_bExist || !state->m_bAttached)
    {
      continue;
    }

    CLuaEventNode *eventNode = m_LuaEventArEx.GetAtPtr(index);
    if (!eventNode || !eventNode->IsCallFun())
    {
      continue;
    }

    if (eventNode->GetNextLoopTime() <= GetLoopTime())
    {
      CLuaScript *script = eventNode->GetScript();
      if (script)
      {
        const char *name = eventNode->GetCallFunName();
        lua_tinker::call<void, CLuaEventNode *>(script->GetLuaState(), name, eventNode);
      }

      eventNode->SetNextLoopTime(GetLoopTime() + eventNode->GetTimeTerm());
    }
  }
}

CLuaEventNode *CLuaEventMgr::NewEvent()
{
  const unsigned int index =
    US::CArrayEx<CLuaEventNode, CLuaEventNode::_State>::SearchSlotIndex<US::CArrayEx<CLuaEventNode, CLuaEventNode::_State>::ORDER_INC>(
      &m_LuaEventArEx,
      &CLuaEventNode::_State::ms_cEmpty);
  if (index == static_cast<unsigned int>(-1))
  {
    return nullptr;
  }

  CLuaEventNode::_State *state = m_LuaEventArEx.GetStateAtPtr(index);
  CLuaEventNode *eventNode = m_LuaEventArEx.GetAtPtr(index);
  if (!state || !eventNode)
  {
    return nullptr;
  }

  state->m_bExist = true;
  state->m_bAttached = false;
  return eventNode;
}

void CLuaEventMgr::RemoveEvent(CLuaEventNode *pEvent)
{
  const unsigned int index = m_LuaEventArEx.GetIndex(pEvent);
  if (index == static_cast<unsigned int>(-1))
  {
    return;
  }

  CLuaEventNode::_State *state = m_LuaEventArEx.GetStateAtPtr(index);
  if (state)
  {
    state->m_bExist = false;
    state->m_bAttached = false;
  }
}

bool CLuaEventMgr::AttachEvent(CLuaEventNode *pEvent)
{
  const unsigned int index = m_LuaEventArEx.GetIndex(pEvent);
  if (index == static_cast<unsigned int>(-1))
  {
    return false;
  }

  CLuaEventNode::_State *state = m_LuaEventArEx.GetStateAtPtr(index);
  if (!state)
  {
    return false;
  }

  state->m_bAttached = true;
  return true;
}

bool CLuaEventMgr::DettachEvent(CLuaEventNode *pEvent)
{
  if (!pEvent)
  {
    return false;
  }

  pEvent->Init();
  RemoveEvent(pEvent);
  return true;
}

CLuaEventNode *CLuaEventMgr::SearchEvent(const char *strFileName)
{
  const unsigned int size = m_LuaEventArEx.GetSize();
  for (unsigned int index = 0; index < size; ++index)
  {
    CLuaEventNode::_State *state = m_LuaEventArEx.GetStateAtPtr(index);
    if (!state || !state->m_bExist || !state->m_bAttached)
    {
      continue;
    }

    CLuaEventNode *eventNode = m_LuaEventArEx.GetAtPtr(index);
    if (eventNode && std::strcmp(eventNode->GetName(), strFileName) == 0)
    {
      return eventNode;
    }
  }

  return nullptr;
}
