#pragma once

#include "IdaCompat.h"

#ifndef _MFC_VER
class __cppobj CNotSupportedException
{
public:
  virtual ~CNotSupportedException() = default;
};
#endif
