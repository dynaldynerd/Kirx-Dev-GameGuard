#include "pch.h"

#include "LuaShim.h"

#include <cstdio>
#include <cstdlib>

void *l_alloc(void *, void *ptr, size_t, size_t nsize)
{
  if (nsize)
  {
    return realloc(ptr, nsize);
  }
  free(ptr);
  return nullptr;
}

int panic(lua_State *L)
{
  const char *message = lua_tolstring(L, -1, nullptr);
  fprintf(stderr, "PANIC: unprotected error in call to Lua API (%s)\n", message);
  return 0;
}

lua_State *lua_my_open()
{
  lua_State *state = lua_newstate(l_alloc, nullptr);
  if (state)
  {
    lua_atpanic(state, panic);
  }
  return state;
}
