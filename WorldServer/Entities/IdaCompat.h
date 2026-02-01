#pragma once

// IDA-specific annotations used in decompiled headers.
#ifndef __cppobj
#define __cppobj
#endif

#ifndef __unaligned
#define __unaligned
#endif

#ifndef __bitmask
#define __bitmask
#endif

#include <cstddef>
#include <cstring>
#include <cmath>

// IDA decompiler helpers (map to standard C/C++ runtime).
inline void *memset_0(void *dest, int ch, size_t count) { return std::memset(dest, ch, count); }
inline void *memcpy_0(void *dest, const void *src, size_t count) { return std::memcpy(dest, src, count); }
inline size_t strlen_0(const char *str) { return std::strlen(str); }
inline int strcmp_0(const char *a, const char *b) { return std::strcmp(a, b); }
inline int strncmp_0(const char *a, const char *b, size_t count) { return std::strncmp(a, b, count); }
inline float sqrtf_0(float v) { return std::sqrt(v); }
