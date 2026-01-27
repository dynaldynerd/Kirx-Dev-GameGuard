#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct __cppobj _CheckPotion_fld : _base_fld
{
  struct _CheckEffectCode
  {
    int m_nContParamCode;
    int m_nContParamIndex;
    float m_fContValue;
    int m_nContValSt;
  };

  char m_strKOR_name[64];
  int m_nContEffectType;
  int m_nContAreaType;
  _CheckEffectCode m_CheckEffectCode[5];
};
#pragma pack(pop)
