#pragma once

#include "IdaCompat.h"

class __cppobj CGoldenBoxItemMgr
{
public:
  struct _BoxItemOpen_output
  {
    unsigned int m_dwOutputIndex;
    unsigned int m_dwEffectCode;
  };

  virtual ~CGoldenBoxItemMgr() = default;
};

