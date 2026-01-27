#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct __cppobj _player_fld : _base_fld
{
  char m_strName[64];
  int m_nRaceCode;
  int m_nSexCode;
  float m_fMoveWalkRate;
  float m_fMoveRunRate;
  float m_fHeight;
  float m_fWidth;
  float m_fAttackRange;
  float m_fDefGap;
  float m_fDefFacing;
};
#pragma pack(pop)
