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

#ifndef BYTE4
#define BYTE4(x) (*((unsigned char *)&(x) + 4))
#endif

// IDA decompiler helpers (map to standard C/C++ runtime).
inline void *memset_0(void *dest, int ch, size_t count) { return std::memset(dest, ch, count); }
inline void *memcpy_0(void *dest, const void *src, size_t count) { return std::memcpy(dest, src, count); }
inline size_t strlen_0(const char *str) { return std::strlen(str); }
inline int strcmp_0(const char *a, const char *b) { return std::strcmp(a, b); }
inline int strncmp_0(const char *a, const char *b, size_t count) { return std::strncmp(a, b, count); }
inline float sqrtf_0(float v) { return std::sqrt(v); }
inline float sinf_0(float v) { return std::sinf(v); }
inline float cosf_0(float v) { return std::cosf(v); }
inline float acosf_0(float v) { return std::acosf(v); }
inline float asinf_0(float v) { return std::asinf(v); }
inline double sin_0(double v) { return std::sin(v); }
inline double cos_0(double v) { return std::cos(v); }
inline double asin_0(double v) { return std::asin(v); }

// Common float constants referenced by IDA output.
static constexpr float FLOAT_0_5 = 0.5f;
static constexpr float FLOAT_1_0 = 1.0f;
static constexpr float FLOAT_N1_0 = -1.0f;
static constexpr float FLOAT_8_0 = 8.0f;
static constexpr float FLOAT_255_0 = 255.0f;
static constexpr float FLOAT_32767_0 = 32767.0f;
