#include <cstdarg>
#include <cstdio>
#include <stdexcept>

#if defined(_M_IX86)
#pragma comment(linker, "/alternatename:__snprintf=___snprintf")
#pragma comment(linker, "/alternatename:__vsnprintf=___vsnprintf")
#endif

extern "C" int __cdecl __vsnprintf(char *buffer, size_t count, const char *format, va_list args)
{
  return vsnprintf(buffer, count, format, args);
}

extern "C" int __cdecl __snprintf(char *buffer, size_t count, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  const int result = __vsnprintf(buffer, count, format, args);
  va_end(args);
  return result;
}

namespace std
{
void __cdecl _Xran(void)
{
  throw out_of_range("invalid string position");
}

void __cdecl _Xlen(void)
{
  throw length_error("string too long");
}
}
