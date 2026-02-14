#include "pch.h"

#include "CLuaScriptMgr.h"

#include "CLuaCommand.h"
#include "CLuaCommandEx.h"
#include "CLuaEventMgr.h"
#include "CLuaEventNode.h"
#include "CLuaLootingMgr.h"
#include "CLuaScript.h"
#include "CLuaSignalReActor.h"
#include "CMapOperation.h"
#include "CMonster.h"
#include "GlobalObjects.h"
#include "LuaParam3.h"
#include "LuaShim.h"
#include "LuaTinker.h"
#include "WorldServerUtil.h"

#include <assert.h>
#include <crtdbg.h>
#include <cstring>
#include <cstdio>
#include <new>
#include <Windows.h>

namespace
{
const char kLuaScriptLogDir[] = "..\\ZoneServerLog\\LuaScriptLog";
const char kLuaMasterStateFile[] = ".\\LuaScript\\MasterState.lua";
} // namespace

CLuaScriptMgr *CLuaScriptMgr::ms_Instance = nullptr;

CLuaEventMgr *_GetLuaEventMgr()
{
  return CLuaEventMgr::Instance();
}

CMonster *_CreateMon(char *strMapName, char *monCode, float fX, float fY, float fZ);
void _CreateLootingNovusItem(char *strItemCode, char *strMapName, LuaParam3 *pos, LuaParam3 *param);

int LuaScripAlert(lua_State *L)
{
  const char *message = lua_tolstring(L, -1, nullptr);
  MyMessageBox("Lua Script Error", "%s", message ? message : "");

  CLuaScriptMgr *mgr = CLuaScriptMgr::Instance();
  CLuaScript *script = mgr ? mgr->SearchScriptFromLuaState(L) : nullptr;
  CLogFile *errorLog = mgr ? mgr->GetErrorLogFile() : nullptr;
  if (script && errorLog)
  {
    errorLog->Write("%s", message ? message : "");
    errorLog->Write("************  Lua Alert start( %s )  **************", script->GetName());
    mgr->LogStack(script);
    errorLog->Write("************  Lua Alert end ( %s )  **************", script->GetName());
  }

  return 0;
}

CLuaScriptMgr::CLuaScriptMgr()
  : m_MasterState(nullptr)
{
  m_ChildScriptArEx.Alloc(0x14u);
  m_ExternCommandArEx.Alloc(0x3E8u);
}

CLuaScriptMgr::~CLuaScriptMgr()
{
  if (m_MasterState)
  {
    lua_close(m_MasterState);
  }
}

CLuaScriptMgr *CLuaScriptMgr::Instance()
{
  if (!CLuaScriptMgr::ms_Instance)
  {
    CLuaScriptMgr::ms_Instance = new (std::nothrow) CLuaScriptMgr();
  }
  return CLuaScriptMgr::ms_Instance;
}

void CLuaScriptMgr::Destroy()
{
  if (CLuaScriptMgr::ms_Instance)
  {
    delete CLuaScriptMgr::ms_Instance;
    CLuaScriptMgr::ms_Instance = nullptr;
  }

  CLuaEventMgr::Destroy();
  CLuaLootingMgr::Destroy();
}

bool CLuaScriptMgr::InitSDM()
{
  CLuaEventMgr *eventMgr = CLuaEventMgr::Instance();
  if (!eventMgr || !eventMgr->InitSDM())
  {
    return false;
  }

  CLuaLootingMgr *lootingMgr = CLuaLootingMgr::Instance();
  if (!lootingMgr || !lootingMgr->InitSDM(0xAu, 0x64u))
  {
    return false;
  }

  CreateDirectoryA(kLuaScriptLogDir, nullptr);

  char dest[256]{};
  const unsigned int errorTime = GetKorLocalTime();
  sprintf_s(dest, "..\\ZoneServerLog\\LuaScriptLog\\%LuaScriptError%d.log", errorTime);
  m_LogScriptError.SetWriteLogFile(dest, 1, 1, 1, 1);

  memset_0(dest, 0, sizeof(dest));
  const unsigned int stateTime = GetKorLocalTime();
  sprintf_s(dest, "..\\ZoneServerLog\\LuaScriptLog\\%LuaScriptState%d.log", stateTime);
  m_LogScriptState.SetWriteLogFile(dest, 1, 1, 1, 1);

  m_MasterState = lua_my_open();
  luaopen_base(m_MasterState);
  luaopen_string(m_MasterState);
  luaopen_table(m_MasterState);
  luaopen_math(m_MasterState);
  lua_settop(m_MasterState, 0);
  lua_pushcclosure(m_MasterState, LuaScripAlert, 0);
  lua_setfield(m_MasterState, LUA_GLOBALSINDEX, "_ALERT");

  if (!_Regist_Novus())
  {
    return false;
  }

  lua_tinker::dofile(m_MasterState, kLuaMasterStateFile);
  return true;
}

bool CLuaScriptMgr::_Regist_Novus()
{
  lua_tinker::class_add<LuaParam3>(m_MasterState, "Param3");
  lua_tinker::constructor<int, int, int, void> ctor[24]{};
  memset(ctor, 0, 1u);
  lua_tinker::class_con<LuaParam3, lua_tinker::constructor<int, int, int, void>>(m_MasterState, ctor[0]);
  lua_tinker::class_mem<LuaParam3, LuaParam3, int>(m_MasterState, "m_1", reinterpret_cast<int *>(0));
  lua_tinker::class_mem<LuaParam3, LuaParam3, int>(m_MasterState, "m_2", reinterpret_cast<int *>(4));
  lua_tinker::class_mem<LuaParam3, LuaParam3, int>(m_MasterState, "m_3", reinterpret_cast<int *>(8));

  lua_tinker::class_add<CLuaScript>(m_MasterState, "Script");
  lua_tinker::class_def<CLuaScript, const char *(CLuaScript::*)(void)>(m_MasterState, "GetName", &CLuaScript::GetName);

  lua_tinker::class_add<CLuaEventNode>(m_MasterState, "Event");
  lua_tinker::class_def<CLuaEventNode, void (CLuaEventNode::*)(const char *)>(m_MasterState, "SetName", &CLuaEventNode::SetName);
  lua_tinker::class_def<CLuaEventNode, void (CLuaEventNode::*)(const char *, unsigned long)>(
    m_MasterState,
    "SetCallFun",
    &CLuaEventNode::SetCallFun);
  lua_tinker::class_def<CLuaEventNode, void (CLuaEventNode::*)(CLuaScript *)>(m_MasterState, "SetScript", &CLuaEventNode::SetScript);

  lua_tinker::class_add<CLuaEventMgr>(m_MasterState, "EventMgr");
  lua_tinker::class_def<CLuaEventMgr, CLuaEventNode *(CLuaEventMgr::*)(void)>(m_MasterState, "NewEvent", &CLuaEventMgr::NewEvent);
  lua_tinker::class_def<CLuaEventMgr, void (CLuaEventMgr::*)(CLuaEventNode *)>(m_MasterState, "RemoveEvent", &CLuaEventMgr::RemoveEvent);
  lua_tinker::class_def<CLuaEventMgr, bool (CLuaEventMgr::*)(CLuaEventNode *)>(m_MasterState, "AttachEvent", &CLuaEventMgr::AttachEvent);
  lua_tinker::class_def<CLuaEventMgr, bool (CLuaEventMgr::*)(CLuaEventNode *)>(m_MasterState, "DettachEvent", &CLuaEventMgr::DettachEvent);
  lua_tinker::class_def<CLuaEventMgr, CLuaEventNode *(CLuaEventMgr::*)(const char *)>(
    m_MasterState,
    "SearchEvent",
    &CLuaEventMgr::SearchEvent);

  lua_tinker::class_add<CLuaSignalReActor>(m_MasterState, "SignalReActor");
  lua_tinker::class_def<CLuaSignalReActor, bool (CLuaSignalReActor::*)(unsigned char, unsigned char, const char *, const char *)>(
    m_MasterState,
    "AddAction",
    &CLuaSignalReActor::AddAction);

  lua_tinker::class_add<CMonster>(m_MasterState, "Monster");
  lua_tinker::class_def<CMonster, CLuaSignalReActor *(CMonster::*)(void)>(m_MasterState, "GetSignalReActor", &CMonster::GetSignalReActor);

  lua_tinker::def<CLuaEventMgr *(*)(void)>(m_MasterState, "GetEventMgr", _GetLuaEventMgr);
  lua_tinker::def<CMonster *(*)(char *, char *, float, float, float)>(m_MasterState, "CreateMon", _CreateMon);
  lua_tinker::def<void (*)(char *, char *, LuaParam3 *, LuaParam3 *)>(
    m_MasterState,
    "CreateNovusItem",
    _CreateLootingNovusItem);

  return true;
}

CLuaScript *CLuaScriptMgr::NewScript()
{
  const unsigned int index =
    US::CArrayEx<CLuaScript, CLuaScript::_State>::SearchSlotIndex<US::CArrayEx<CLuaScript, CLuaScript::_State>::ORDER_INC>(
      &m_ChildScriptArEx,
      &CLuaScript::_State::ms_cEmpty);
  if (index == static_cast<unsigned int>(-1))
  {
    return nullptr;
  }

  CLuaScript::_State *state = m_ChildScriptArEx.GetStateAtPtr(index);
  CLuaScript *script = m_ChildScriptArEx.GetAtPtr(index);
  if (!state || !script)
  {
    return nullptr;
  }

  state->m_bExist = true;
  state->m_bAttached = false;
  return script;
}

CLuaScript *CLuaScriptMgr::SearchScript(const char *name)
{
  if (!name)
  {
    return nullptr;
  }

  const unsigned int size = m_ChildScriptArEx.GetSize();
  for (unsigned int index = 0; index < size; ++index)
  {
    CLuaScript::_State *state = m_ChildScriptArEx.GetStateAtPtr(index);
    if (state && state->m_bExist && state->m_bAttached)
    {
      CLuaScript *script = m_ChildScriptArEx.GetAtPtr(index);
      if (script && strcmp_0(script->m_strName, name) == 0)
      {
        return script;
      }
    }
  }

  return nullptr;
}

CLuaScript *CLuaScriptMgr::SearchScriptFromLuaState(lua_State *state)
{
  if (!state)
  {
    return nullptr;
  }

  const unsigned int size = m_ChildScriptArEx.GetSize();
  for (unsigned int index = 0; index < size; ++index)
  {
    CLuaScript::_State *scriptState = m_ChildScriptArEx.GetStateAtPtr(index);
    if (scriptState && scriptState->m_bExist && scriptState->m_bAttached)
    {
      CLuaScript *script = m_ChildScriptArEx.GetAtPtr(index);
      if (script && script->GetLuaState() == state)
      {
        return script;
      }
    }
  }

  return nullptr;
}

CLuaCommandEx *CLuaScriptMgr::NewCommandEx()
{
  const unsigned int index = US::CArrayEx<CLuaCommandEx, CLuaCommandEx::_State>::SearchSlotIndex<
    US::CArrayEx<CLuaCommandEx, CLuaCommandEx::_State>::ORDER_INC>(&m_ExternCommandArEx, &CLuaCommandEx::_State::ms_cEmpty);
  if (index == static_cast<unsigned int>(-1))
  {
    return nullptr;
  }

  CLuaCommandEx::_State *state = m_ExternCommandArEx.GetStateAtPtr(index);
  CLuaCommandEx *command = m_ExternCommandArEx.GetAtPtr(index);
  if (!state || !command)
  {
    return nullptr;
  }

  state->m_bExist = true;
  return command;
}

void CLuaScriptMgr::RemoveCommandEx(CLuaCommandEx *cmd)
{
  const unsigned int index = m_ExternCommandArEx.GetIndex(cmd);
  if (index == static_cast<unsigned int>(-1))
  {
    return;
  }

  CLuaCommandEx::_State *state = m_ExternCommandArEx.GetStateAtPtr(index);
  if (state)
  {
    state->m_bExist = false;
  }
}

void CLuaScriptMgr::RemoveScript(CLuaScript *pScript)
{
  const unsigned int index = m_ChildScriptArEx.GetIndex(pScript);
  if (index == static_cast<unsigned int>(-1))
  {
    return;
  }

  CLuaScript::_State *state = m_ChildScriptArEx.GetStateAtPtr(index);
  if (state)
  {
    state->m_bExist = false;
    state->m_bAttached = false;
  }
}

bool CLuaScriptMgr::AttachLuaScript(CLuaScript *pScript, CLuaCommand *pAttachCommand)
{
  if (!m_MasterState)
  {
    _wassert(
      L"m_MasterState",
      L"G:\\00_ZoneServer_Source\\03_Temp_Source\\2009_05_13_Source_Oversea\\zoneserver\\GameMain\\LuaScriptMgr\\LuaScriptMgr.cpp",
      0x11Du);
  }

  if (!pScript)
  {
    return false;
  }

  const char *name = pScript->GetName();
  if (SearchScript(name) || strcmp_0("MasterState", pScript->GetName()) == 0)
  {
    RemoveScript(pScript);
    return false;
  }

  const unsigned int index = m_ChildScriptArEx.GetIndex(pScript);
  if (index == static_cast<unsigned int>(-1))
  {
    return false;
  }

  lua_pushstring(m_MasterState, pScript->m_strName);
  pScript->m_MyState = lua_newthread(m_MasterState);
  lua_settable(m_MasterState, LUA_REGISTRYINDEX);

  CLuaScript::_State *state = m_ChildScriptArEx.GetStateAtPtr(index);
  if (state)
  {
    state->m_bAttached = true;
  }

  m_LogScriptState.Write("Attach ( %s )", pScript->GetName());
  if (pAttachCommand)
  {
    pScript->RunCommand(pAttachCommand);
  }

  char buffer[80]{};
  sprintf_s(buffer, 0x40u, "%s_%s", pScript->GetName(), "OnAttach");
  lua_tinker::call<void, CLuaScript *>(pScript->m_MyState, buffer, pScript);
  return true;
}

bool CLuaScriptMgr::DetackLuaScript(CLuaScript *pScript)
{
  if (!m_MasterState)
  {
    _wassert(
      L"m_MasterState",
      L"G:\\00_ZoneServer_Source\\03_Temp_Source\\2009_05_13_Source_Oversea\\zoneserver\\GameMain\\LuaScriptMgr\\LuaScriptMgr.cpp",
      0x14Au);
  }

  if (!pScript)
  {
    return false;
  }

  char buffer[80]{};
  sprintf_s(buffer, 0x40u, "%s_%s", pScript->GetName(), "OnDettach");
  lua_tinker::call<void, CLuaScript *>(pScript->m_MyState, buffer, pScript);

  lua_settop(pScript->m_MyState, 0);
  lua_pushstring(m_MasterState, pScript->GetName());
  lua_pushnil(m_MasterState);
  lua_settable(m_MasterState, LUA_REGISTRYINDEX);

  m_LogScriptState.Write("Dettach ( %s )", pScript->GetName());
  RemoveScript(pScript);
  return true;
}

void CLuaScriptMgr::LogStack(CLuaScript *pScript)
{
  if (!pScript || !pScript->GetLuaState())
  {
    return;
  }

  lua_State *state = pScript->GetLuaState();
  m_LogScriptError.Write("============  Lua Stack start ( %s )  ============", pScript->GetName());
  const int top = lua_gettop(state);
  m_LogScriptError.Write("Type:%d", top);

  for (unsigned int index = 1; index <= static_cast<unsigned int>(lua_gettop(state)); ++index)
  {
    const int type = lua_type(state, static_cast<int>(index));
    switch (type)
    {
      case 0:
      {
        const char *name = lua_typename(state, lua_type(state, static_cast<int>(index)));
        m_LogScriptError.Write("\t%s", name);
        break;
      }
      case 1:
      {
        const char *boolStr = lua_toboolean(state, static_cast<int>(index)) ? "true" : "false";
        const char *name = lua_typename(state, lua_type(state, static_cast<int>(index)));
        m_LogScriptError.Write("\t%s\t%s", name, boolStr);
        break;
      }
      case 2:
      {
        const void *ptr = lua_topointer(state, static_cast<int>(index));
        const char *name = lua_typename(state, lua_type(state, static_cast<int>(index)));
        m_LogScriptError.Write("\t%s\t0x%08p", name, ptr);
        break;
      }
      case 3:
      {
        const double num = lua_tonumber(state, static_cast<int>(index));
        const char *name = lua_typename(state, lua_type(state, static_cast<int>(index)));
        m_LogScriptError.Write("\t%s\t%f", name, num);
        break;
      }
      case 4:
      {
        const char *str = lua_tolstring(state, static_cast<int>(index), nullptr);
        const char *name = lua_typename(state, lua_type(state, static_cast<int>(index)));
        m_LogScriptError.Write("\t%s\t%s", name, str);
        break;
      }
      case 5:
      {
        const void *ptr = lua_topointer(state, static_cast<int>(index));
        const char *name = lua_typename(state, lua_type(state, static_cast<int>(index)));
        m_LogScriptError.Write("\t%s\t0x%08p", name, ptr);
        break;
      }
      case 6:
      {
        const void *ptr = lua_topointer(state, static_cast<int>(index));
        const char *name = lua_typename(state, lua_type(state, static_cast<int>(index)));
        m_LogScriptError.Write("\t%s()\t0x%08p", name, ptr);
        break;
      }
      case 7:
      {
        const void *ptr = lua_topointer(state, static_cast<int>(index));
        const char *name = lua_typename(state, lua_type(state, static_cast<int>(index)));
        m_LogScriptError.Write("\t%s\t0x%08p", name, ptr);
        break;
      }
      case 8:
      {
        const char *name = lua_typename(state, lua_type(state, static_cast<int>(index)));
        m_LogScriptError.Write("\t%s", name);
        break;
      }
      default:
        break;
    }
  }

  m_LogScriptError.Write("============  Lua Stack End ( %s )  ==============", pScript->GetName());
}

void CLuaScriptMgr::Loop()
{
  CLuaEventMgr *eventMgr = CLuaEventMgr::Instance();
  if (eventMgr)
  {
    eventMgr->Loop();
  }

  CLuaLootingMgr *lootingMgr = CLuaLootingMgr::Instance();
  if (lootingMgr)
  {
    lootingMgr->Loop();
  }
}

CLogFile *CLuaScriptMgr::GetStateLogFile()
{
  return &m_LogScriptState;
}

CLogFile *CLuaScriptMgr::GetErrorLogFile()
{
  return &m_LogScriptError;
}

CMonster *_CreateMon(char *strMapName, char *monCode, float fX, float fY, float fZ)
{
  float pos[3]{fX, fY, fZ};
  CMapData *map = g_MapOper.GetMap(strMapName);
  if (!map)
  {
    return nullptr;
  }

  return CreateRepMonster(map, 0, pos, monCode, nullptr, false, true, false, false, true);
}

void _CreateLootingNovusItem(char *strItemCode, char *strMapName, LuaParam3 *pos, LuaParam3 *param)
{
  float fPos[3]{static_cast<float>(pos->m_1), static_cast<float>(pos->m_2), static_cast<float>(pos->m_3)};
  CMapData *map = g_MapOper.GetMap(strMapName);
  if (!map)
  {
    return;
  }

  const unsigned __int16 lootRange = static_cast<unsigned __int16>(param->m_1);
  const unsigned int overlapCount = static_cast<unsigned int>(param->m_2);
  const unsigned int itemCount = static_cast<unsigned int>(param->m_3);

  CLuaLootingMgr *mgr = CLuaLootingMgr::Instance();
  if (mgr)
  {
    mgr->AddNovusItem(strItemCode, map, 0, fPos, lootRange, overlapCount, itemCount, 0);
  }
}
