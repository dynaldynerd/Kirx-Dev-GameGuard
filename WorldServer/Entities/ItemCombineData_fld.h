#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)

struct _ItemCombine_fld : _base_fld
{
  struct _material
  {
    char m_itmPdMat[8];
    int m_nDur;
  };

  int m_CombineIdx;
  unsigned int m_dwFee;
  char m_strCivil[64];
  int m_TradeValue;
  unsigned int m_dwTradeMoney;
  _material m_Material[5];
};

#pragma pack(pop)

using _ItemCombineData_fld = _ItemCombine_fld;
