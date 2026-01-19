#pragma once

#include "IdaCompat.h"

class __cppobj CIP
{
public:
  virtual ~CIP() = default;

  union
  {
    unsigned int m_dwIP;
    unsigned short m_wPortPair[2];
  };
};

