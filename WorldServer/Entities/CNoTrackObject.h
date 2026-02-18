#pragma once

#include "IdaCompat.h"

#ifndef _MFC_VER
class  CNoTrackObject
{
public:
  virtual ~CNoTrackObject() = default;
};
#endif
