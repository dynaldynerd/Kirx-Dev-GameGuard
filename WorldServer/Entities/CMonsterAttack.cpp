#include "pch.h"

#include "CMonsterAttack.h"

#include "CCharacter.h"
#include "CLogFile.h"
#include "CMonster.h"
#include "force_fld.h"
#include "skill_fld.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

#include <cstdlib>

CMonsterAttack::CMonsterAttack(CMonster *pThis)
  : CAttack(pThis),
    m_pAttMonster(pThis)
{
}

float CMonsterAttack::ModifyMonsterAttFc(float fAttFc)
{
  if ((1.0f - fAttFc) <= 0.0f)
  {
    return fAttFc;
  }
  return 1.0f - (float)((1.0f - fAttFc) * 0.25f);
}

void CMonsterAttack::AttackMonsterGen(_attack_param *pParam, bool bMustMiss)
{
  m_nDamagedObjNum = 0;
  m_bIsCrtAtt = false;
  m_pp = pParam;
  bool hitSuccess = true;
  m_nDamagedObjNum = 0;
  m_pAttChar->BreakStealth();

  if (m_pp->pDst)
  {
    bool isAvoidState = false;
    if (m_pp->pDst->m_EP.GetEff_State(14))
    {
      isAvoidState = true;
    }
    else if (m_pp->pDst->m_EP.GetEff_Plus(27) > 0.0f)
    {
      const float randValue = static_cast<float>(rand() % 100);
      const float avoidRate = m_pp->pDst->m_EP.GetEff_Plus(27);
      if (avoidRate > randValue)
      {
        isAvoidState = true;
      }
    }

    if (isAvoidState)
    {
      if (!m_pp->bPassCount && !m_pp->nClass && !m_pp->pDst->GetWeaponClass())
      {
        const float targetRange = m_pp->pDst->GetAttackRange();
        const float attackerWidth = m_pAttChar->GetWidth();
        const float maxRange = targetRange + (attackerWidth / 2.0f);
        const float rangeBonus = m_pp->pDst->m_EP.GetEff_Plus(4);
        const float limitRange = maxRange + rangeBonus;
        const float distance = GetSqrt(m_pp->pDst->m_fCurPos, m_pAttChar->m_fCurPos);
        if (limitRange >= distance)
        {
          m_DamList[0].m_pChar = m_pp->pDst;
          m_DamList[0].m_nDamage = -1;
          m_nDamagedObjNum = 1;
          m_pAttChar->SendMsg_AttackActEffect(0, m_pp->pDst);
          return;
        }
      }

      if (m_pp->pDst->m_EP.GetEff_Plus(27) > 0.0f)
      {
        m_DamList[0].m_pChar = m_pp->pDst;
        m_DamList[0].m_nDamage = 0;
        m_nDamagedObjNum = 1;
        return;
      }
    }

    if (m_pp->bMatchless)
    {
      m_DamList[0].m_pChar = m_pp->pDst;
      m_DamList[0].m_nDamage = m_pp->pDst->GetHP();
      m_nDamagedObjNum = 1;
      return;
    }

    if (m_pp->pDst->m_EP.GetEff_State(8))
    {
      hitSuccess = false;
    }
    else
    {
      const int randValue = rand() % 100;
      const int attackProb = m_pAttChar->GetGenAttackProb(m_pp->pDst, m_pp->nPart, m_pp->bBackAttack);
      if (randValue >= attackProb)
      {
        hitSuccess = false;
      }
    }

    if (hitSuccess && bMustMiss)
    {
      hitSuccess = false;
    }

    if (!hitSuccess)
    {
      m_DamList[0].m_pChar = m_pp->pDst;
      m_DamList[0].m_nDamage = 0;
      m_nDamagedObjNum = 1;
      return;
    }
  }

  float attackPower = static_cast<float>(m_pp->nAddAttPnt + _CalcGenAttPnt(false));
  float attackRate = FLOAT_1_0;
  if (m_pp->nWpType == 7)
  {
    attackRate = m_pAttChar->m_EP.GetEff_Rate(29);
  }
  else
  {
    attackRate = m_pAttChar->m_EP.GetEff_Rate(m_pp->nClass);
  }
  attackPower *= ModifyMonsterAttFc(attackRate);

  const int attackType = pParam->nAttactType;
  if (attackType < 0)
  {
    m_DamList[0].m_pChar = m_pp->pDst;
    m_DamList[0].m_nDamage = 0;
    m_nDamagedObjNum = 1;
    CalcAvgDamage();
    return;
  }

  if (attackType > 2)
  {
    if (attackType == 5)
    {
      if (m_pp->nExtentRange > 0)
      {
        FlashDamageProc(m_pp->nExtentRange, static_cast<int>(attackPower), 90, 0, false);
      }
    }
    else if (attackType == 6)
    {
      if (m_pp->nExtentRange > 0)
      {
        AreaDamageProc(m_pp->nExtentRange, static_cast<int>(attackPower), m_pp->fArea, 0, false);
      }
    }
    else
    {
      m_DamList[0].m_pChar = m_pp->pDst;
      m_DamList[0].m_nDamage = 0;
      m_nDamagedObjNum = 1;
    }
    CalcAvgDamage();
    return;
  }

  m_DamList[0].m_pChar = m_pp->pDst;
  const int attackPoint = static_cast<int>(attackPower);
  m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(
    attackPoint,
    m_pp->nPart,
    m_pp->nTol,
    m_pp->pDst,
    m_pp->bBackAttack);
  m_nDamagedObjNum = 1;
  CalcAvgDamage();
}

void CMonsterAttack::AttackMonsterSkill(_attack_param *pParam)
{
  m_bActiveSucc = false;
  m_nDamagedObjNum = 0;
  m_bIsCrtAtt = false;
  m_pp = pParam;
  _skill_fld *skillField = reinterpret_cast<_skill_fld *>(m_pp->pFld);
  bool hitSuccess = true;
  m_pAttChar->BreakStealth();

  int skillType = 0;
  if (m_pp->byEffectCode)
  {
    skillType = skillField->m_nAttackable;
  }
  else if (m_pp->nLevel <= 1)
  {
    skillType = skillField->m_nAttackable;
  }
  else
  {
    skillType = skillField->m_nAttType[m_pp->nLevel - 2];
  }

  if (!m_pp->pDst)
  {
    goto APPLY_DAMAGE;
  }

  {
    bool isAvoidState = false;
    if (m_pp->pDst->m_EP.GetEff_State(14))
    {
      isAvoidState = true;
    }
    else if (m_pp->pDst->m_EP.GetEff_Plus(27) > 0.0f)
    {
      const float randValue = static_cast<float>(rand() % 100);
      const float avoidRate = m_pp->pDst->m_EP.GetEff_Plus(27);
      if (avoidRate > randValue)
      {
        isAvoidState = true;
      }
    }

    if (isAvoidState)
    {
      if (!m_pp->bPassCount && !m_pp->nClass && !m_pp->pDst->GetWeaponClass())
      {
        const float targetRange = m_pp->pDst->GetAttackRange();
        const float attackerWidth = m_pAttChar->GetWidth();
        const float maxRange = targetRange + (attackerWidth / 2.0f);
        const float rangeBonus = m_pp->pDst->m_EP.GetEff_Plus(4);
        const float limitRange = maxRange + rangeBonus;
        const float distance = GetSqrt(m_pp->pDst->m_fCurPos, m_pAttChar->m_fCurPos);
        if (limitRange >= distance)
        {
          m_DamList[0].m_pChar = m_pp->pDst;
          m_DamList[0].m_nDamage = -1;
          m_nDamagedObjNum = 1;
          m_pAttChar->SendMsg_AttackActEffect(0, m_pp->pDst);
          return;
        }
      }

      if (m_pp->pDst->m_EP.GetEff_Plus(27) > 0.0f)
      {
        m_DamList[0].m_pChar = m_pp->pDst;
        m_DamList[0].m_nDamage = 0;
        m_nDamagedObjNum = 1;
        return;
      }
    }
  }

  if (m_pp->pDst->m_EP.GetEff_State(8))
  {
    hitSuccess = false;
  }
  else
  {
    const float accValue = m_pAttChar->m_EP.GetEff_Plus(30) + 150.0f;
    const int avoidRate = m_pp->pDst->GetAvoidRate();
    int checkValue = static_cast<int>(accValue - static_cast<float>(avoidRate));
    if (checkValue <= 0)
    {
      checkValue = 0;
    }
    if (checkValue >= 100)
    {
      checkValue = 100;
    }
    if (rand() % 100 >= checkValue)
    {
      hitSuccess = false;
    }
  }

  if (!hitSuccess)
  {
    m_DamList[0].m_pChar = m_pp->pDst;
    m_DamList[0].m_nDamage = 0;
    m_nDamagedObjNum = 1;
    return;
  }

APPLY_DAMAGE:
  {
    const int attackPower = m_pp->nAddAttPnt + static_cast<int>(_CalcMonSkillAttPnt());
    switch (skillType)
    {
      case 0:
      case 1:
      case 2:
      case 3:
        if (m_pp->pDst)
        {
          m_DamList[0].m_pChar = m_pp->pDst;
          m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(
            attackPower,
            m_pp->nPart,
            m_pp->nTol,
            m_pp->pDst,
            m_pp->bBackAttack);
          m_nDamagedObjNum = 1;
        }
        break;
      case 4:
      case 6:
        AreaDamageProc(
          s_Mon_nLimitRadius[skillField->m_nLv],
          attackPower,
          m_pp->fArea,
          0,
          false);
        break;
      case 5:
      {
        const int index = skillField->m_nLv;
        FlashDamageProc(s_Mon_nLimitDist[index], attackPower, s_Mon_nLimitAngle[0][index], 0, false);
        break;
      }
      case 7:
        SectorDamageProc(
          skillField->m_nLv,
          attackPower,
          s_Mon_nLimitAngle[0][skillField->m_nLv],
          m_pp->nShotNum,
          m_pp->nExtentRange,
          0,
          false);
        break;
      default:
        return;
    }

    CalcAvgDamage();
  }
}

void CMonsterAttack::AttackMonsterForce(_attack_param *pParam)
{
  m_nDamagedObjNum = 0;
  m_bIsCrtAtt = false;
  m_pp = pParam;
  _force_fld *forceField = reinterpret_cast<_force_fld *>(m_pp->pFld);
  bool hitSuccess = true;
  m_pAttChar->BreakStealth();

  if (m_pp->pDst)
  {
    if (m_pp->pDst->m_EP.GetEff_State(8))
    {
      hitSuccess = false;
    }
    else
    {
      const float accValue = m_pAttChar->m_EP.GetEff_Plus(31) + 100.0f;
      const int avoidRate = m_pp->pDst->GetAvoidRate();
      int checkValue = static_cast<int>(accValue - static_cast<float>(avoidRate));
      if (checkValue <= 0)
      {
        checkValue = 0;
      }
      if (checkValue >= 100)
      {
        checkValue = 100;
      }
      if (rand() % 100 >= checkValue)
      {
        hitSuccess = false;
      }
    }
  }

  if (hitSuccess)
  {
    float attackPower = static_cast<float>(_CalcForceAttPnt(false));
    const float forceRate = g_MonsterSetInfoData.GetMonsterForcePowerRate();
    attackPower *= forceRate;
    const float attFc = m_pAttChar->m_EP.GetEff_Rate(4);
    attackPower *= ModifyMonsterAttFc(attFc);

    const int skillType = forceField->m_nEffectGroup;
    if (skillType < 0)
    {
      return;
    }

    if (skillType <= 2)
    {
      if (m_pp->pDst)
      {
        m_DamList[0].m_pChar = m_pp->pDst;
        m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(
          static_cast<int>(attackPower),
          m_pp->nPart,
          m_pp->nTol,
          m_pp->pDst,
          m_pp->bBackAttack);
        m_nDamagedObjNum = 1;
      }
      CalcAvgDamage();
      return;
    }

    if (skillType == 5)
    {
      const int index = forceField->m_nLv;
      FlashDamageProc(s_Mon_nLimitDist[index], static_cast<int>(attackPower), s_Mon_nLimitAngle[1][index], 0, false);
      CalcAvgDamage();
      return;
    }

    if (skillType != 4 && skillType != 6)
    {
      return;
    }

    const int index = forceField->m_nLv;
    AreaDamageProc(s_Mon_nLimitRadius[index], static_cast<int>(attackPower), m_pp->fArea, 0, false);
    CalcAvgDamage();
    return;
  }

  if (m_pp->pDst)
  {
    m_DamList[0].m_pChar = m_pp->pDst;
    m_DamList[0].m_nDamage = 0;
    m_nDamagedObjNum = 1;
  }
}

__int64 CMonsterAttack::_CalcMonSkillAttPnt()
{
  _skill_fld *skillField = reinterpret_cast<_skill_fld *>(m_pp->pFld);
  const float levelFactor = static_cast<float>(m_pp->nLevel) + ((7.0f - static_cast<float>(m_pp->nLevel)) * 0.5f);
  const float skillConst = skillField->m_fAttFormulaConstant;
  const int levelConst = skillField->m_nAttConstant[m_pp->nLevel - 1];

  unsigned int minAttack = static_cast<unsigned int>(
    static_cast<int>((((levelConst / 788.0f)
                       * (static_cast<float>(m_pp->nMinAF)
                          * (fR + ((levelFactor / 7.0f) * fRLf)
                             + ((static_cast<float>(m_pp->nMastery) / 99.0f) * fRMf)))
                       * skillConst)
                      + 0.5f)));
  int maxAttack = static_cast<int>(
    (((levelConst / 788.0f)
      * (static_cast<float>(m_pp->nMaxAF)
         * (fR + ((levelFactor / 7.0f) * fRLf)
            + ((static_cast<float>(m_pp->nMastery) / 99.0f) * fRMf)))
      * skillConst)
     + 0.5f));

  if (maxAttack < 0)
  {
    g_Main.m_logSystemError.Write(
      "Skill Attack Error : Skill(%s), SIndex(%d), l_fConst(%f), l_nLvConst(%d), nMastery(%d), nMaxAF(%d), nMinAF(%d)",
      skillField->m_strCode,
      skillField->m_dwIndex,
      skillConst,
      levelConst,
      m_pp->nMastery,
      m_pp->nMaxAF,
      m_pp->nMinAF);
    minAttack = 1;
    maxAttack = 1;
  }

  const unsigned int criticalAttack = static_cast<unsigned int>(
    static_cast<int>(((static_cast<double>((maxAttack + 125) / (maxAttack + 50) * maxAttack)) + 0.5)));

  if (m_pp->nMaxAttackPnt > 0)
  {
    return criticalAttack;
  }
  if (m_pp->nMaxAttackPnt < 0)
  {
    return minAttack;
  }

  const unsigned int midAttack = static_cast<unsigned int>(
    static_cast<int>(((static_cast<float>(maxAttack + minAttack) / 2.0f) + 0.5f)));

  const int randValue = m_pAttChar->m_rtPer100.GetRand();

  int minSel = static_cast<int>(static_cast<float>(m_pp->nMinSel) - m_pAttChar->m_EP.GetEff_Plus(14));
  if (m_pp->pDst && m_pp->pDst != m_pAttChar)
  {
    minSel = static_cast<int>(static_cast<float>(minSel) + m_pp->pDst->m_EP.GetEff_Plus(37));
  }
  if (minSel < 0)
  {
    minSel = 0;
  }

  int maxSel = m_pp->nMaxSel + m_pp->nMinSel;
  if (m_pp->pDst && m_pp->pDst != m_pAttChar)
  {
    maxSel = static_cast<int>(static_cast<float>(maxSel) + m_pp->pDst->m_EP.GetEff_Plus(37));
  }
  if (maxSel < 0)
  {
    maxSel = 0;
  }

  if (randValue >= minSel)
  {
    if (randValue >= maxSel)
    {
      m_bIsCrtAtt = true;
      return criticalAttack;
    }
    if (maxAttack - static_cast<int>(midAttack) <= 0)
    {
      return midAttack;
    }
    return (rand() % (maxAttack - static_cast<int>(midAttack))) + midAttack;
  }

  if (static_cast<int>(midAttack) - static_cast<int>(minAttack) <= 0)
  {
    return minAttack;
  }
  return (rand() % (static_cast<int>(midAttack) - static_cast<int>(minAttack))) + minAttack;
}

