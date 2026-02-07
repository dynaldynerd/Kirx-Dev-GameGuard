#pragma once

#include "IdaCompat.h"

#include <cstddef>

struct lua_State;

using lua_CFunction = int (*)(lua_State *);

extern "C"
{
  lua_State *lua_my_open();
  void luaopen_base(lua_State *L);
  void luaopen_string(lua_State *L);
  void luaopen_table(lua_State *L);
  void luaopen_math(lua_State *L);
  void lua_close(lua_State *L);

  void lua_settop(lua_State *L, int idx);
  void lua_pushcclosure(lua_State *L, lua_CFunction fn, int n);
  void lua_setfield(lua_State *L, long long idx, const char *k);
  void lua_pushstring(lua_State *L, const char *s);
  lua_State *lua_newthread(lua_State *L);
  void lua_settable(lua_State *L, long long idx);
  void lua_pushnil(lua_State *L);

  int lua_gettop(lua_State *L);
  int lua_type(lua_State *L, int idx);
  const char *lua_typename(lua_State *L, int tp);
  int lua_toboolean(lua_State *L, int idx);
  const void *lua_topointer(lua_State *L, int idx);
  double lua_tonumber(lua_State *L, int idx);
  const char *lua_tolstring(lua_State *L, int idx, size_t *len);
}
