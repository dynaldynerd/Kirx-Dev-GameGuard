#include "pch.h"

#include "LuaShim.h"

struct lua_State
{
  int dummy;
};

namespace
{
lua_State g_dummyLuaState{};
const char kLuaUnknownType[] = "unknown";
const char kLuaEmptyString[] = "";
} // namespace

extern "C"
{
  lua_State *lua_my_open()
  {
    return &g_dummyLuaState;
  }

  void luaopen_base(lua_State *) {}
  void luaopen_string(lua_State *) {}
  void luaopen_table(lua_State *) {}
  void luaopen_math(lua_State *) {}
  void lua_close(lua_State *) {}

  void lua_settop(lua_State *, int) {}
  void lua_pushcclosure(lua_State *, lua_CFunction, int) {}
  void lua_setfield(lua_State *, long long, const char *) {}
  void lua_pushstring(lua_State *, const char *) {}
  lua_State *lua_newthread(lua_State *) { return &g_dummyLuaState; }
  void lua_settable(lua_State *, long long) {}
  void lua_pushnil(lua_State *) {}

  int lua_gettop(lua_State *) { return 0; }
  int lua_type(lua_State *, int) { return 0; }
  const char *lua_typename(lua_State *, int) { return kLuaUnknownType; }
  int lua_toboolean(lua_State *, int) { return 0; }
  const void *lua_topointer(lua_State *, int) { return nullptr; }
  double lua_tonumber(lua_State *, int) { return 0.0; }
  const char *lua_tolstring(lua_State *, int, size_t *len)
  {
    if (len)
    {
      *len = 0;
    }
    return kLuaEmptyString;
  }
}
