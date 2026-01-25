#include "pch.h"

#include "CMonsterSkill.h"

void CMonsterSkill::Init()
{
  m_bExit = false;
  m_UseType = -1;
  m_nSFCode = -1;
  m_wSFIndex = 0;
  m_pSF_Fld = nullptr;
  m_BefTime = 0;
  m_dwDelayTime = 0;
  m_fAttackDist = 0.0f;
  m_dwCastDelay = 0;
  m_nSFLv = 1;
  m_Element = -1;
  m_StdDmg = 0;
  m_MinDmg = 0;
  m_MaxDmg = 0;
  m_MinProb = 0;
  m_MaxProb = 100;
  m_nMotive = 0;
  m_nMotivevalue = -1;
  m_nCaseType = 0;
  m_nAccumulationCount = 0;
  m_pSPConst = nullptr;
}

