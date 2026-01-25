#pragma once

#include "IdaCompat.h"

template <typename T>
class __cppobj CTSingleton
{
public:
  static T *Instance()
  {
    static T s_instance;
    return &s_instance;
  }

  virtual ~CTSingleton() = default;
};
