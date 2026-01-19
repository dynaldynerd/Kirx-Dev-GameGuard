#pragma once

#include "IdaCompat.h"

#ifndef _MFC_VER
class __cppobj CNoTrackObject
{
public:
  virtual ~CNoTrackObject() = default;
};
#endif
