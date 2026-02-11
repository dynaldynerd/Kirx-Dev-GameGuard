#pragma once

#include "IdaCompat.h"

struct __cppobj _unit_bullet_param
{
  unsigned __int16 wBulletIndex;
  unsigned __int16 wLeftNum;

  bool IsFill() const
  {
    return *reinterpret_cast<const unsigned int *>(this) != 0xFFFFFFFFu;
  }
};
