#pragma once

#include "IdaCompat.h"

#include <cstddef>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

lua_State *lua_my_open();
void *l_alloc(void *ud, void *ptr, size_t osize, size_t nsize);
int panic(lua_State *L);
