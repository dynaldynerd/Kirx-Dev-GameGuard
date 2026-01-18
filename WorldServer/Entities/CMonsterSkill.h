#pragma once

#include "IdaCompat.h"

struct _base_fld;
struct _monster_sp_fld;

/* 1647 */
class __cppobj __declspec(align(8)) CMonsterSkill
{
  bool m_bExit;
  int m_UseType;
  int m_nSFCode;
  unsigned __int16 m_wSFIndex;
  _base_fld *m_pSF_Fld;
  _monster_sp_fld *m_pSPConst;
  unsigned int m_BefTime;
  unsigned int m_dwDelayTime;
  unsigned int m_dwCastDelay;
  float m_fAttackDist;
  int m_nMotive;
  int m_nMotivevalue;
  int m_nCaseType;
  int m_nAccumulationCount;
  int m_nSFLv;
  int m_Element;
  int m_StdDmg;
  int m_MinDmg;
  int m_MaxDmg;
  int m_MinProb;
  int m_MaxProb;
};

