#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct __cppobj _monster_sp_fld : _base_fld
{
  int m_nSpecialAttType;
  char m_strSpecialAttCode[64];
  int m_nAttLv;
  float m_fSpecialRange;
  int m_nMotiveCondition;
  int m_nMotiveValue;
  int m_nMotiveExceptionCondition;
  int m_nMotiveExceptionValue;
  int m_nMotiveTime;
  int m_nDstType;
  int m_nLimitCount;
  int m_nProbability;
};
#pragma pack(pop)
