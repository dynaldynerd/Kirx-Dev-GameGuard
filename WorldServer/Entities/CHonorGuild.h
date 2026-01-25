#pragma once

#include "IdaCompat.h"

class __cppobj CHonorGuild
{
public:
  static CHonorGuild *Instance();
  bool Init();

  virtual ~CHonorGuild() = default;

  unsigned int m_dwGuildSerial;
  unsigned int m_dwTotalHonor;
};
