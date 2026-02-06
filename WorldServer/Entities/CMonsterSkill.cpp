#include "pch.h"

#include "CMonsterSkill.h"

#include "force_fld.h"
#include "monster_fld.h"
#include "monster_sp_fld.h"
#include "skill_fld.h"

#include <cstdlib>

CMonsterSkill::CMonsterSkill()
{
  Init();
}

CMonsterSkill::~CMonsterSkill() = default;

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

void CMonsterSkill::Copy(const CMonsterSkill *other)
{
  m_bExit = other->m_bExit;
  m_UseType = other->m_UseType;
  m_nSFCode = other->m_nSFCode;
  m_wSFIndex = other->m_wSFIndex;
  m_pSF_Fld = other->m_pSF_Fld;
  m_BefTime = other->m_BefTime;
  m_dwDelayTime = other->m_dwDelayTime;
  m_fAttackDist = other->m_fAttackDist;
  m_dwCastDelay = other->m_dwCastDelay;
  m_nSFLv = other->m_nSFLv;
  m_Element = other->m_Element;
  m_StdDmg = other->m_StdDmg;
  m_MinDmg = other->m_MinDmg;
  m_MaxDmg = other->m_MaxDmg;
  m_MinProb = other->m_MinProb;
  m_MaxProb = other->m_MaxProb;
  m_nMotive = other->m_nMotive;
  m_nMotivevalue = other->m_nMotivevalue;
  m_nCaseType = other->m_nCaseType;
  m_nAccumulationCount = other->m_nAccumulationCount;
  m_pSPConst = other->m_pSPConst;
}

__int64 CMonsterSkill::SetSkill(
  _monster_fld *pMonsterFld,
  _monster_sp_fld *pSPCont,
  int nSFLv,
  int nEffectType,
  _skill_fld *pSkillFld,
  unsigned int dwDelayTime,
  float fAttackDist,
  unsigned int dwCastDelay,
  int nMotive,
  int nMotiveValue,
  int skillDestType)
{
  Init();
  m_nSFCode = (nEffectType == 2) ? 2 : 1;
  m_pSF_Fld = pSkillFld;
  m_pSPConst = pSPCont;
  if (!m_pSF_Fld)
  {
    Init();
    return 0;
  }

  m_wSFIndex = m_pSF_Fld->m_dwIndex;
  m_dwDelayTime = dwDelayTime;
  m_fAttackDist = fAttackDist;
  m_dwCastDelay = dwCastDelay;
  m_nSFLv = nSFLv;
  m_Element = pMonsterFld->m_nProperty;
  m_StdDmg = static_cast<int>(pMonsterFld->m_fAttFcStd);
  const int minStdDmg = static_cast<int>(static_cast<float>(m_StdDmg) * 0.89999998f);
  if (m_StdDmg - minStdDmg <= 0)
  {
    m_MinDmg = 0;
  }
  else
  {
    m_MinDmg = rand() % (m_StdDmg - minStdDmg) + minStdDmg;
  }
  m_MaxDmg = 2 * m_StdDmg - m_MinDmg;
  const float damageRatio = static_cast<float>(m_MaxDmg + 125) / static_cast<float>(m_MaxDmg + 50);
  (void)damageRatio;
  m_MinProb = static_cast<int>(pMonsterFld->m_fMinAFSelProb);
  m_MaxProb = static_cast<int>(pMonsterFld->m_fMaxAFSelProb);
  if (nEffectType)
  {
    if (nEffectType == 2)
    {
      m_UseType = _Check_SF_UseType(pSkillFld, 2);
    }
  }
  else
  {
    m_UseType = _Check_SF_UseType(pSkillFld, 0);
  }
  m_nMotive = nMotive;
  m_nMotivevalue = nMotiveValue;
  m_nCaseType = skillDestType;
  m_bExit = true;
  return 1;
}

__int64 CMonsterSkill::SetForce(
  _monster_fld *pMonsterFld,
  _monster_sp_fld *pSPCont,
  int nSFLv,
  _force_fld *pForceFld,
  unsigned int dwDelayTime,
  float fAttackDist,
  unsigned int dwCastDelay,
  int nMotive,
  int nMotiveValue,
  int skillDestType)
{
  Init();
  m_nSFCode = 3;
  m_pSF_Fld = pForceFld;
  m_pSPConst = pSPCont;
  if (!m_pSF_Fld)
  {
    Init();
    return 0;
  }

  m_wSFIndex = m_pSF_Fld->m_dwIndex;
  m_dwDelayTime = dwDelayTime;
  m_fAttackDist = fAttackDist;
  m_dwCastDelay = dwCastDelay;
  m_nSFLv = nSFLv;
  m_Element = pForceFld->m_nProperty;
  m_StdDmg = static_cast<int>(pMonsterFld->m_fAttFcStd);
  const int minStdDmg = static_cast<int>(static_cast<float>(m_StdDmg) * 0.89999998f);
  if (m_StdDmg - minStdDmg <= 0)
  {
    m_MinDmg = 0;
  }
  else
  {
    m_MinDmg = rand() % (m_StdDmg - minStdDmg) + minStdDmg;
  }
  m_MaxDmg = 2 * m_StdDmg - m_MinDmg;
  const float damageRatio = static_cast<float>(m_MaxDmg + 125) / static_cast<float>(m_MaxDmg + 50);
  (void)damageRatio;
  m_MinProb = static_cast<int>(pMonsterFld->m_fMinAFSelProb);
  m_MaxProb = static_cast<int>(pMonsterFld->m_fMaxAFSelProb);
  m_UseType = _Check_SF_UseType(pForceFld, 1);
  m_nMotive = nMotive;
  m_nMotivevalue = nMotiveValue;
  m_nCaseType = skillDestType;
  m_bExit = true;
  return 1;
}

__int64 CMonsterSkill::SetGen(
  _monster_fld *pMonsterFld,
  int nSFLv,
  unsigned int dwDelayTime,
  float fAttackDist,
  unsigned int dwCastDelay)
{
  Init();
  m_nSFCode = 0;
  m_wSFIndex = 0;
  m_pSF_Fld = nullptr;
  m_dwDelayTime = dwDelayTime;
  m_fAttackDist = fAttackDist;
  m_dwCastDelay = dwCastDelay;
  m_nSFLv = nSFLv;
  m_Element = pMonsterFld->m_nProperty;
  m_StdDmg = static_cast<int>(pMonsterFld->m_fAttFcStd);
  const int minStdDmg = static_cast<int>(static_cast<float>(m_StdDmg) * 0.89999998f);
  if (m_StdDmg - minStdDmg <= 0)
  {
    m_MinDmg = 0;
  }
  else
  {
    m_MinDmg = rand() % (m_StdDmg - minStdDmg) + minStdDmg;
  }
  m_MaxDmg = 2 * m_StdDmg - m_MinDmg;
  const float damageRatio = static_cast<float>(m_MaxDmg + 125) / static_cast<float>(m_MaxDmg + 50);
  (void)damageRatio;
  m_MinProb = static_cast<int>(pMonsterFld->m_fMinAFSelProb);
  m_MaxProb = static_cast<int>(pMonsterFld->m_fMaxAFSelProb);
  m_UseType = 0;
  m_nMotive = 0;
  m_nMotivevalue = -1;
  m_nCaseType = 0;
  m_bExit = true;
  if (m_bExit)
  {
    return 1;
  }
  Init();
  return 0;
}

__int64 CMonsterSkill::Use(unsigned int dwUsedTime, bool bCount)
{
  m_BefTime = dwUsedTime;
  if (bCount)
  {
    SetAccumulationCountAdd(1);
  }
  return 0;
}

bool CMonsterSkill::IsExit()
{
  return m_bExit;
}

bool CMonsterSkill::IsAttackAble()
{
  return m_UseType == 0;
}

__int64 CMonsterSkill::GetType()
{
  return static_cast<unsigned int>(m_nSFCode);
}

__int64 CMonsterSkill::GetUseType()
{
  return static_cast<unsigned int>(m_UseType);
}

__int64 CMonsterSkill::GetMotive()
{
  return static_cast<unsigned int>(m_nMotive);
}

__int64 CMonsterSkill::GetMotiveValue()
{
  return static_cast<unsigned int>(m_nMotivevalue);
}

__int64 CMonsterSkill::GetExceptMotive()
{
  if (m_pSPConst)
  {
    return static_cast<unsigned int>(m_pSPConst->m_nMotiveExceptionCondition);
  }
  return 0xFFFFFFFFu;
}

__int64 CMonsterSkill::GetExceptMotiveValue()
{
  if (m_pSPConst)
  {
    return static_cast<unsigned int>(m_pSPConst->m_nMotiveExceptionValue);
  }
  return 0xFFFFFFFFu;
}

__int64 CMonsterSkill::GetDstCaseType()
{
  return static_cast<unsigned int>(m_nCaseType);
}

__int64 CMonsterSkill::GetBeforeTime()
{
  return m_BefTime;
}

__int64 CMonsterSkill::GetNextActionDelayTime()
{
  return m_dwDelayTime;
}

__int64 CMonsterSkill::GetSPActionProbability()
{
  if (m_pSPConst)
  {
    return static_cast<unsigned int>(100 * m_pSPConst->m_nProbability);
  }
  return 0;
}

__int64 CMonsterSkill::GetSPLimitCount()
{
  if (m_pSPConst)
  {
    return static_cast<unsigned int>(m_pSPConst->m_nLimitCount);
  }
  return 0;
}

__int64 CMonsterSkill::GetSFLv()
{
  return static_cast<unsigned int>(m_nSFLv);
}

__int64 CMonsterSkill::GetElement()
{
  return static_cast<unsigned int>(m_Element);
}

__int64 CMonsterSkill::GetStdDmg()
{
  return static_cast<unsigned int>(m_StdDmg);
}

__int64 CMonsterSkill::GetMinDmg()
{
  return static_cast<unsigned int>(m_MinDmg);
}

__int64 CMonsterSkill::GetMaxDmg()
{
  return static_cast<unsigned int>(m_MaxDmg);
}

__int64 CMonsterSkill::GetMinProb()
{
  return static_cast<unsigned int>(m_MinProb);
}

__int64 CMonsterSkill::GetMaxProb()
{
  return static_cast<unsigned int>(m_MaxProb);
}

float CMonsterSkill::GetAttackDist()
{
  return m_fAttackDist;
}

_base_fld *CMonsterSkill::GetFld()
{
  return m_pSF_Fld;
}

void CMonsterSkill::NextPass()
{
  m_BefTime += m_dwDelayTime;
}

void CMonsterSkill::SetAccumulationCountAdd(int nTempAccumulationCount)
{
  m_nAccumulationCount += nTempAccumulationCount;
}

__int64 CMonsterSkill::GetAccumulationCount()
{
  return static_cast<unsigned int>(m_nAccumulationCount);
}

int _Check_SF_UseType(_base_fld *pFld, int nEffectType)
{
  if (!pFld)
  {
    return -1;
  }

  if (nEffectType && nEffectType != 2)
  {
    if (nEffectType == 1)
    {
      const _base_fld *record = pFld;
      if (record[11].m_dwIndex)
      {
        return 0;
      }

      const int conditionValue = *reinterpret_cast<const int *>(&record[12].m_strCode[48]);
      if (conditionValue == -1)
      {
        if (*reinterpret_cast<const int *>(&record[12].m_strCode[12]) == -1)
        {
          return -1;
        }
        if (conditionValue >= 150)
        {
          return -1;
        }
        return 3;
      }

      if (conditionValue)
      {
        if (conditionValue == 1)
        {
          return 2;
        }
        return -1;
      }
      return 1;
    }
    return -1;
  }

  const _base_fld *record = pFld;
  if (record[11].m_dwIndex == 1)
  {
    return 0;
  }

  const int conditionValue = *reinterpret_cast<const int *>(&record[13].m_strCode[32]);
  if (conditionValue == -1)
  {
    if (record[13].m_dwIndex == -1)
    {
      return -1;
    }
    if (conditionValue >= 150)
    {
      return -1;
    }
    return 3;
  }

  if (conditionValue)
  {
    if (conditionValue == 1)
    {
      return 2;
    }
    return -1;
  }
  return 1;
}

