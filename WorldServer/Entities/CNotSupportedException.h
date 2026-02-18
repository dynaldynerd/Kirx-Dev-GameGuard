#pragma once

#include "IdaCompat.h"

#ifndef _MFC_VER
class  CNotSupportedException
{
public:
  virtual ~CNotSupportedException() = default;
};
#endif
