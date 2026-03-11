#pragma once

// IDA-specific annotations used in decompiled headers.
//#ifndef 
//#define 
//#endif

//#if !defined(_MSC_VER)
//#ifndef 
//#define 
//#endif
//#endif

//#ifndef __bitmask
//#define __bitmask
//#endif

#include <cstddef>
#include <cstring>
#include <cmath>
#include <limits>

#include "IdaFloatConstants.h"

#ifndef BYTE4
#define BYTE4(x) (*((unsigned char *)&(x) + 4))
#endif

#ifndef BYTE2
#define BYTE2(x) (*((unsigned char *)&(x) + 2))
#endif

#ifndef BYTE1
#define BYTE1(x) (*((unsigned char *)&(x) + 1))
#endif

#ifndef LOBYTE
#define LOBYTE(x) (*((unsigned char *)&(x)))
#endif

#ifndef HIBYTE
#define HIBYTE(x) (*((unsigned char *)&(x) + 1))
#endif

#ifndef SLOBYTE
#define SLOBYTE(x) (*((signed char *)&(x)))
#endif

#ifndef LODWORD
#define LODWORD(x) (*((unsigned int *)&(x)))
#endif

inline size_t saturated_mul(size_t a, size_t b)
{
  if (a == 0 || b == 0)
  {
    return 0;
  }
  if (a > (std::numeric_limits<size_t>::max)() / b)
  {
    return (std::numeric_limits<size_t>::max)();
  }
  return a * b;
}

