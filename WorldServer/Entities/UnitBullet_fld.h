#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct  _UnitBullet_fld : _base_fld
{
  char m_strModle[64];
  int m_nIconIDX;
  char m_strName[64];
  int m_nWPType;
  int m_bAbr;
  int m_nDurUnit;
  int m_nMoney;
  int m_nStdPrice;
  int m_nStdPoint;
  float m_fGAAF;
  char m_strEffectIndex[64];
  char m_strTooltipIndex[64];
};
#pragma pack(pop)
