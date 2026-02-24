#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)

struct _ItemCombineData_fld : _base_fld
{
  struct _material
  {
    char m_itmPdMat[8];
    int m_nUpt;
    int m_nDur;
  };

  int m_nReserved;
  unsigned int m_dwFee;
  char m_strCivil[64];
  int m_nEventType;
  unsigned int m_dwNeedActPoint;
  _material m_Material[5];
};

#pragma pack(pop)
