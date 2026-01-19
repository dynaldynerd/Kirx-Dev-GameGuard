#pragma once

#include "IdaCompat.h"

class __cppobj CHashMapPtrPool
{
public:
  virtual ~CHashMapPtrPool() = default;

  void *m_pPool;
  unsigned int m_dwEntryCapacity;
  unsigned int m_dwEntryCount;
};

