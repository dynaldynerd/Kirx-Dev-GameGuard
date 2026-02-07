#pragma once

#include "IdaCompat.h"

#include <type_traits>

struct lua_State;

namespace lua_tinker
{
template <typename... Args>
struct constructor
{
};

inline void dofile(lua_State *, const char *) {}
inline void dostring(lua_State *, const char *) {}

template <typename Ret, typename... Args, std::enable_if_t<!std::is_void_v<Ret>, int> = 0>
inline Ret call(lua_State *, const char *, Args...)
{
  return Ret();
}

template <typename Ret = void, typename... Args, std::enable_if_t<std::is_void_v<Ret>, int> = 0>
inline void call(lua_State *, const char *, Args...)
{
}

template <typename Class>
inline void class_add(lua_State *, const char *)
{
}

template <typename Class, typename Constructor>
inline void class_con(lua_State *, Constructor)
{
}

template <typename Class, typename Access, typename Type>
inline void class_mem(lua_State *, const char *, Type *)
{
}

template <typename Class, typename Func>
inline void class_def(lua_State *, const char *, Func)
{
}

template <typename Func>
inline void def(lua_State *, const char *, Func)
{
}
} // namespace lua_tinker
