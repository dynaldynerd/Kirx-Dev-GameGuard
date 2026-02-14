#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __cppobj _nuclear_result_code_zocl
{
  unsigned __int8 byRetCode;

  _nuclear_result_code_zocl()
  {
    byRetCode = 0;
  }

  unsigned __int16 size() const
  {
    return 1;
  }
};
#pragma pack(pop)
