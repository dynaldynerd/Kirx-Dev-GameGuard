#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)

struct _ItemExchangeData_fld : _base_fld
{
  #pragma pack(push, 1)

  struct _output
  {
    char m_strOutCode[8];
    unsigned __int8 m_byDur;
    unsigned __int8 m_byPadding[3];
    unsigned int m_dwProb;
  };
  #pragma pack(pop)
  _output m_listOutput[61];
};

#pragma pack(pop)
