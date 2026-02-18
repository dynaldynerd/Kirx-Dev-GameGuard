#pragma once

#include "IdaCompat.h"

template <typename T>
class  CTSingleton
{
public:
  static T *Instance()
  {
    static T s_instance;
    return &s_instance;
  }

  virtual ~CTSingleton() = default;
};
