#include "pch.h"

#include "WorldServerUtil.h"

#include "CPlayerAttack.h"
#include "CHolyStoneSystem.h"
#include "CLogFile.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CPlayer.h"
#include "UnitPart_fld.h"
#include "force_fld.h"
#include "skill_fld.h"
#include "GlobalObjects.h"

CPlayerAttack::CPlayerAttack(CPlayer *pThis)
  : CAttack(pThis),
    m_pAttPlayer(pThis)
{
}

__int64 CPlayerAttack::_CalcSkillAttPnt(bool bUseEffBullet)
{
  _skill_fld *skillRecord = reinterpret_cast<_skill_fld *>(m_pp->pFld);
  const float levelFactor =
    static_cast<float>(m_pp->nLevel) + (static_cast<float>(7.0f - static_cast<float>(m_pp->nLevel)) * 0.5f);
  const float fConst = skillRecord->m_fAttFormulaConstant;
  const int lvConst = skillRecord->m_nAttConstant[m_pp->nLevel - 1];

  unsigned int minAttack = 0;
  int maxAttack = 5;
  if (bUseEffBullet)
  {
    minAttack =
      static_cast<int>(
        ((static_cast<float>(lvConst) / 788.0f)
          * ((static_cast<float>(m_pp->nMinAFPlus)
               * (fR_1 + ((levelFactor / 7.0f) * fRLf_1)
                 + ((static_cast<float>(m_pp->nMastery) / 99.0f) * fRMf_1)))
            * fConst))
          + 0.5f);
    const float maxBase =
      (static_cast<float>(m_pp->nMaxAFPlus)
        * (fR_1 + ((levelFactor / 7.0f) * fRLf_1) + ((static_cast<float>(m_pp->nMastery) / 99.0f) * fRMf_1)))
      * fConst;
    maxAttack = static_cast<int>(((static_cast<float>(lvConst) / 788.0f) * maxBase) + 0.5f);
  }
  else
  {
    minAttack =
      static_cast<int>(
        ((static_cast<float>(lvConst) / 788.0f)
          * ((static_cast<float>(m_pp->nMinAF)
               * (fR_1 + ((levelFactor / 7.0f) * fRLf_1)
                 + ((static_cast<float>(m_pp->nMastery) / 99.0f) * fRMf_1)))
            * fConst))
          + 0.5f);
    const float maxBase =
      (static_cast<float>(m_pp->nMaxAF)
        * (fR_1 + ((levelFactor / 7.0f) * fRLf_1) + ((static_cast<float>(m_pp->nMastery) / 99.0f) * fRMf_1)))
      * fConst;
    maxAttack = static_cast<int>(((static_cast<float>(lvConst) / 788.0f) * maxBase) + 0.5f);
  }

  if (maxAttack < 0)
  {
    g_Main.m_logSystemError.Write(
      "Skill Attack Error : Skill(%s), SIndex(%d), l_fConst(%f), l_nLvConst(%d), nMastery(%d), nMaxAF(%d), nMinAF(%d)",
      skillRecord->m_strCode,
      skillRecord->m_dwIndex,
      fConst,
      lvConst,
      m_pp->nMastery,
      m_pp->nMaxAF,
      m_pp->nMinAF);
    minAttack = 1;
    maxAttack = 1;
  }

  const unsigned int maxDamage =
    static_cast<int>(((static_cast<double>(maxAttack + 125) / static_cast<double>(maxAttack + 50)) * maxAttack) + 0.5);
  if (m_pp->nMaxAttackPnt > 0)
  {
    return maxDamage;
  }
  if (m_pp->nMaxAttackPnt < 0)
  {
    return minAttack;
  }

  const unsigned int midAttack = static_cast<int>(((static_cast<float>(maxAttack + minAttack) / 2.0f) + 0.5f));
  const int randValue = m_pAttChar->m_rtPer100.GetRand();

  int minSel = static_cast<int>(static_cast<float>(m_pp->nMinSel) - m_pAttChar->m_EP.GetEff_Plus(EFF_PLUS_CRITICAL_RATE));
  if (m_pp->pDst && m_pp->pDst != m_pAttChar)
  {
    minSel = static_cast<int>(static_cast<float>(minSel) + m_pp->pDst->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_37));
    minSel += MonsterCritical_Exception_Rate(reinterpret_cast<CMonster *>(m_pp->pDst), m_pp->bBackAttack);
  }
  if (minSel < 0)
  {
    minSel = 0;
  }

  int maxSel =
    static_cast<int>(static_cast<float>(m_pp->nMaxSel + m_pp->nMinSel) - m_pAttChar->m_EP.GetEff_Plus(EFF_PLUS_CRITICAL_RATE));
  if (m_pp->pDst && m_pp->pDst != m_pAttChar)
  {
    maxSel = static_cast<int>(static_cast<float>(maxSel) + m_pp->pDst->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_37));
    maxSel += MonsterCritical_Exception_Rate(reinterpret_cast<CMonster *>(m_pp->pDst), m_pp->bBackAttack);
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
      return maxDamage;
    }
    if ((maxAttack - static_cast<int>(midAttack)) <= 0)
    {
      return midAttack;
    }
    return (rand() % (maxAttack - static_cast<int>(midAttack))) + midAttack;
  }

  if ((static_cast<int>(midAttack) - static_cast<int>(minAttack)) <= 0)
  {
    return minAttack;
  }
  return (rand() % (static_cast<int>(midAttack) - static_cast<int>(minAttack))) + minAttack;
}

void CPlayerAttack::AttackSkill(_attack_param *pParam, bool bUseEffBullet)
{
  m_nDamagedObjNum = 0;
  m_bIsCrtAtt = false;
  m_pp = pParam;
  _skill_fld *skillRecord = reinterpret_cast<_skill_fld *>(m_pp->pFld);
  bool canHit = true;
  m_pAttChar->BreakStealth();

  int attackType = 0;
  if (m_pp->byEffectCode)
  {
    attackType = skillRecord->m_nAttackable;
  }
  else if (m_pp->nLevel <= 1)
  {
    attackType = skillRecord->m_nAttackable;
  }
  else
  {
    attackType = skillRecord->m_nAttType[m_pp->nLevel - 2];
  }

  if (m_pp->pDst)
  {
    bool isAvoided = false;
    if (m_pp->pDst->m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
    {
      isAvoided = true;
    }
    else if (m_pp->pDst->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_27) > 0.0f)
    {
      const float roll = static_cast<float>(rand() % 100);
      if (m_pp->pDst->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_27) > roll)
      {
        isAvoided = true;
      }
    }
    if (isAvoided)
    {
      if (!m_pp->bPassCount && !m_pp->nClass && !m_pp->pDst->GetWeaponClass())
      {
        const float targetRange = m_pp->pDst->GetAttackRange();
        const float attackerWidth = m_pAttChar->GetWidth();
        const float rangeLimit =
          targetRange + (attackerWidth / 2.0f) + m_pp->pDst->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_4);
        const float dist = GetSqrt(m_pp->pDst->m_fCurPos, m_pAttChar->m_fCurPos);
        if (rangeLimit >= dist)
        {
          m_DamList[0].m_pChar = m_pp->pDst;
          m_DamList[0].m_nDamage = -1;
          m_nDamagedObjNum = 1;
          m_pAttChar->SendMsg_AttackActEffect(0, m_pp->pDst);
          return;
        }
      }
      if (m_pp->pDst->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_27) > 0.0f)
      {
        m_DamList[0].m_pChar = m_pp->pDst;
        m_DamList[0].m_nDamage = 0;
        m_nDamagedObjNum = 1;
        return;
      }
    }

    if (m_pp->pDst->m_EP.GetEff_State(EFF_STATE_UNKNOWN_8))
    {
      canHit = false;
    }
    else
    {
      const float baseChance = m_pAttChar->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_30) + 100.0f;
      const int avoid = m_pp->pDst->GetAvoidRate();
      int chance = static_cast<int>(baseChance - static_cast<float>(avoid));
      if (!m_pAttChar->m_ObjID.m_byID)
      {
        const float add = m_pAttChar->m_EP.GetEff_Plus(EFF_PLUS_ATTACK_ACCURACY);
        chance = static_cast<int>(static_cast<float>(chance) + add);
      }
      if (chance < 0)
      {
        chance = 0;
      }
      if (chance > 100)
      {
        chance = 100;
      }
      if (rand() % 100 >= chance)
      {
        canHit = false;
      }
    }

    if (!canHit)
    {
      m_DamList[0].m_pChar = m_pp->pDst;
      m_DamList[0].m_nDamage = 0;
      m_nDamagedObjNum = 1;
      return;
    }
  }

  int normalAttack = m_pp->nAddAttPnt + static_cast<int>(_CalcSkillAttPnt(false));
  int effAttack = m_pp->nAddAttPnt + static_cast<int>(_CalcSkillAttPnt(bUseEffBullet));
  if (!m_pAttChar->m_ObjID.m_byID
    && (g_HolySys.GetDestroyerSerial() == m_pAttChar->m_dwObjSerial
      || reinterpret_cast<CPlayer *>(m_pAttChar)->IsLastAttBuff()))
  {
    normalAttack = static_cast<int>(static_cast<float>(normalAttack) * 1.3f);
    effAttack = static_cast<int>(static_cast<float>(effAttack) * 1.3f);
  }

  if (!m_pAttPlayer->m_bInGuildBattle)
  {
    const unsigned int dwSerial = m_pAttPlayer->m_Param.GetCharSerial();
    const int raceCode = m_pAttPlayer->m_Param.GetRaceCode();
    CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
    const unsigned __int8 bossType = ranking->GetBossType(raceCode, dwSerial);
    if (bossType)
    {
      if (bossType == 2 || bossType == 6)
      {
        normalAttack = static_cast<int>(static_cast<float>(normalAttack) * 1.2f);
        effAttack = static_cast<int>(static_cast<float>(effAttack) * 1.2f);
      }
    }
    else
    {
      normalAttack = static_cast<int>(static_cast<float>(normalAttack) * 1.3f);
      effAttack = static_cast<int>(static_cast<float>(effAttack) * 1.3f);
    }
  }

  if (m_pp->nWpType == 7)
  {
    normalAttack =
      static_cast<int>(static_cast<float>(normalAttack) * m_pAttChar->m_EP.GetEff_Rate(EFF_RATE_LAUNCHER_ATTACK));
    effAttack =
      static_cast<int>(static_cast<float>(effAttack) * m_pAttChar->m_EP.GetEff_Rate(EFF_RATE_LAUNCHER_ATTACK));
  }
  else
  {
    normalAttack =
      static_cast<int>(
        static_cast<float>(normalAttack) * m_pAttChar->m_EP.GetEff_Rate(m_pp->nClass + 2));
    effAttack =
      static_cast<int>(
        static_cast<float>(effAttack) * m_pAttChar->m_EP.GetEff_Rate(m_pp->nClass + 2));
  }

  switch (attackType)
  {
    case 0:
    case 1:
    case 2:
    case 3:
      if (m_pp->pDst)
      {
        m_DamList[0].m_pChar = m_pp->pDst;
        if (bUseEffBullet)
        {
          m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(effAttack,
            m_pp->nPart,
            m_pp->nTol,
            m_pp->pDst,
            m_pp->bBackAttack);
        }
        else
        {
          m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(normalAttack,
            m_pp->nPart,
            m_pp->nTol,
            m_pp->pDst,
            m_pp->bBackAttack);
        }
        m_nDamagedObjNum = 1;
      }
      CalcAvgDamage();
      break;
    case 4:
    case 6:
      AreaDamageProc(
        s_nLimitRadius[skillRecord->m_nLv],
        normalAttack,
        m_pp->fArea,
        effAttack,
        bUseEffBullet);
      CalcAvgDamage();
      break;
    case 5:
    {
      const int index = skillRecord->m_nLv;
      FlashDamageProc(s_nLimitDist[index], normalAttack, s_nLimitAngle[0][index], effAttack, bUseEffBullet);
      CalcAvgDamage();
      break;
    }
    case 7:
      SectorDamageProc(
        skillRecord->m_nLv,
        normalAttack,
        s_nLimitAngle[0][skillRecord->m_nLv],
        m_pp->nShotNum,
        m_pp->nExtentRange,
        effAttack,
        bUseEffBullet);
      CalcAvgDamage();
      break;
    default:
      break;
  }
}

void CPlayerAttack::AttackUnit(_attack_param *pParam)
{
  m_nDamagedObjNum = 0;
  m_bIsCrtAtt = false;
  m_pp = pParam;
  _UnitPart_fld *weaponField = reinterpret_cast<_UnitPart_fld *>(m_pp->pFld);
  bool canHit = true;
  m_pAttChar->BreakStealth();

  if (m_pp->pDst)
  {
    const int effectGroup = weaponField->m_nEffectGroup;
    if (effectGroup != 4)
    {
      bool isAvoided = false;
      if (m_pp->pDst->m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
      {
        isAvoided = true;
      }
      else if (m_pp->pDst->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_27) > 0.0f)
      {
        const float roll = static_cast<float>(rand() % 100);
        if (m_pp->pDst->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_27) > roll)
        {
          isAvoided = true;
        }
      }
      if (isAvoided)
      {
        if (!m_pp->bPassCount && !m_pp->nClass && !m_pp->pDst->GetWeaponClass())
        {
          const float targetRange = m_pp->pDst->GetAttackRange();
          const float attackerWidth = m_pAttChar->GetWidth();
          const float rangeLimit =
            targetRange + (attackerWidth / 2.0f) + m_pp->pDst->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_4);
          const float dist = GetSqrt(m_pp->pDst->m_fCurPos, m_pAttChar->m_fCurPos);
          if (rangeLimit >= dist)
          {
            m_DamList[0].m_pChar = m_pp->pDst;
            m_DamList[0].m_nDamage = -1;
            m_nDamagedObjNum = 1;
            m_pAttChar->SendMsg_AttackActEffect(0, m_pp->pDst);
            return;
          }
        }
        if (m_pp->pDst->m_EP.GetEff_Plus(EFF_PLUS_UNKNOWN_27) > 0.0f)
        {
          m_DamList[0].m_pChar = m_pp->pDst;
          m_DamList[0].m_nDamage = 0;
          m_nDamagedObjNum = 1;
          return;
        }
      }

      if (m_pp->pDst->m_EP.GetEff_State(EFF_STATE_UNKNOWN_8))
      {
        canHit = false;
      }
      else
      {
        const int roll = rand() % 100;
        const int attackProb = m_pAttChar->GetGenAttackProb(m_pp->pDst, m_pp->nPart, m_pp->bBackAttack);
        if (roll >= attackProb)
        {
          canHit = false;
        }
      }

      if (!canHit)
      {
        m_DamList[0].m_pChar = m_pp->pDst;
        m_DamList[0].m_nDamage = 0;
        m_nDamagedObjNum = 1;
        return;
      }
    }

    int normalAttack = m_pp->nAddAttPnt + static_cast<int>(_CalcGenAttPnt(false));
    normalAttack =
      static_cast<int>(static_cast<float>(normalAttack) * m_pAttChar->m_EP.GetEff_Rate(EFF_RATE_UNIT_ATTACK));

    if (!m_pAttPlayer->m_bInGuildBattle)
    {
      const unsigned int dwSerial = m_pAttPlayer->m_Param.GetCharSerial();
      const int raceCode = m_pAttPlayer->m_Param.GetRaceCode();
      CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
      const unsigned __int8 bossType = ranking->GetBossType(raceCode, dwSerial);
      if (bossType)
      {
        if (bossType == 2 || bossType == 6)
        {
          normalAttack = static_cast<int>(static_cast<float>(normalAttack) * 1.2f);
        }
      }
      else
      {
        normalAttack = static_cast<int>(static_cast<float>(normalAttack) * 1.3f);
      }
    }

    float defSum = 0.0f;
    for (int part = 0; part < 5; ++part)
    {
      defSum = static_cast<float>(m_pAttPlayer->GetDefFC(part, m_pAttPlayer, nullptr));
    }
    const float defAvg = defSum / 5.0f;
    const float addDef = defAvg * (m_pAttPlayer->m_EP.GetEff_Rate(EFF_RATE_UNKNOWN_44) - 1.0f);

    const float attackFcA = GetAttackFC(m_pAttPlayer, 2u, true, true);
    const float addAttackA = attackFcA * (m_pAttPlayer->m_EP.GetEff_Rate(EFF_RATE_UNKNOWN_43) - 1.0f);

    const float attackFcB0 = GetAttackFC(m_pAttPlayer, 0, true, true);
    const float attackFcB1 = GetAttackFC(m_pAttPlayer, 1u, true, true);
    const float addAttackB =
      (attackFcB0 + attackFcB1) * (m_pAttPlayer->m_EP.GetEff_Rate(EFF_RATE_UNKNOWN_41) - 1.0f);

    const float attackFcC0 = GetAttackFC(m_pAttPlayer, 0, false, true);
    const float attackFcC1 = GetAttackFC(m_pAttPlayer, 1u, false, true);
    const float addAttackC =
      (attackFcC0 + attackFcC1) * (m_pAttPlayer->m_EP.GetEff_Rate(EFF_RATE_UNKNOWN_42) - 1.0f);

    normalAttack =
      static_cast<int>(static_cast<float>(normalAttack) + addAttackA + addDef + addAttackB + addAttackC);

    if (effectGroup >= 0)
    {
      if (effectGroup <= 3)
      {
        if (m_pp->pDst)
        {
          m_DamList[0].m_pChar = m_pp->pDst;
          m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(normalAttack,
            m_pp->nPart,
            m_pp->nTol,
            m_pp->pDst,
            m_pp->bBackAttack);
          m_nDamagedObjNum = 1;
        }
      }
      else
      {
        switch (effectGroup)
        {
          case 5:
            FlashDamageProc(s_nLimitDist[3], normalAttack, s_nLimitAngle[0][3], 0, false);
            break;
          case 6:
            AreaDamageProc(s_nLimitRadius[3], normalAttack, m_pp->fArea, 0, false);
            break;
          case 7:
            SectorDamageProc(3, normalAttack, s_nLimitAngle[0][3], m_pp->nShotNum, m_pp->nExtentRange, 0, false);
            break;
          default:
            return;
        }
      }
      CalcAvgDamage();
    }
  }
}

void CPlayerAttack::WPActiveAttackSkill(_attack_param *pParam)
{
  m_nDamagedObjNum = 0;
  m_bIsCrtAtt = false;
  m_pp = pParam;
  _skill_fld *skillRecord = reinterpret_cast<_skill_fld *>(m_pp->pFld);
  m_pAttChar->BreakStealth();

  int attackType = 0;
  if (m_pp->byEffectCode)
  {
    attackType = skillRecord->m_nAttackable;
  }
  else if (m_pp->nLevel <= 1)
  {
    attackType = skillRecord->m_nAttackable;
  }
  else
  {
    attackType = skillRecord->m_nAttType[m_pp->nLevel - 2];
  }

  int normalAttack = m_pp->nAddAttPnt + static_cast<int>(_CalcSkillAttPnt(false));
  if (m_pp->nWpType == 7)
  {
    normalAttack =
      static_cast<int>(static_cast<float>(normalAttack) * m_pAttChar->m_EP.GetEff_Rate(EFF_RATE_LAUNCHER_ATTACK));
  }
  else
  {
    normalAttack =
      static_cast<int>(
        static_cast<float>(normalAttack) * m_pAttChar->m_EP.GetEff_Rate(m_pp->nClass + 2));
  }

  if (!m_pAttPlayer->m_bInGuildBattle)
  {
    const unsigned int dwSerial = m_pAttPlayer->m_Param.GetCharSerial();
    const int raceCode = m_pAttPlayer->m_Param.GetRaceCode();
    CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
    const unsigned __int8 bossType = ranking->GetBossType(raceCode, dwSerial);
    if (bossType)
    {
      if (bossType == 2 || bossType == 6)
      {
        normalAttack = static_cast<int>(static_cast<float>(normalAttack) * 1.2f);
      }
    }
    else
    {
      normalAttack = static_cast<int>(static_cast<float>(normalAttack) * 1.3f);
    }
  }

  switch (attackType)
  {
    case 0:
    case 1:
    case 2:
    case 3:
      if (m_pp->pDst)
      {
        m_DamList[0].m_pChar = m_pp->pDst;
        m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(normalAttack,
          m_pp->nPart,
          m_pp->nTol,
          m_pp->pDst,
          m_pp->bBackAttack);
        m_nDamagedObjNum = 1;
      }
      CalcAvgDamage();
      break;
    case 4:
    case 6:
      AreaDamageProc(
        s_nLimitRadius[skillRecord->m_nLv],
        normalAttack,
        m_pp->fArea,
        0,
        false);
      CalcAvgDamage();
      break;
    case 5:
    {
      const int index = skillRecord->m_nLv;
      FlashDamageProc(s_nLimitDist[index], normalAttack, s_nLimitAngle[0][index], 0, false);
      CalcAvgDamage();
      break;
    }
    case 7:
      SectorDamageProc(
        skillRecord->m_nLv,
        normalAttack,
        s_nLimitAngle[0][skillRecord->m_nLv],
        m_pp->nShotNum,
        m_pp->nExtentRange,
        0,
        false);
      CalcAvgDamage();
      break;
    default:
      break;
  }
}

void CPlayerAttack::WPActiveAttackForce(_attack_param *pParam)
{
  m_nDamagedObjNum = 0;
  m_bIsCrtAtt = false;
  m_pp = pParam;
  _force_fld *forceField = reinterpret_cast<_force_fld *>(m_pp->pFld);

  float normalAttack = static_cast<float>(m_pp->nAddAttPnt + _CalcForceAttPnt(false));
  normalAttack *= m_pAttChar->m_EP.GetEff_Rate(EFF_RATE_FORCE_ATTACK);

  if (!m_pAttPlayer->m_bInGuildBattle)
  {
    const unsigned int dwSerial = m_pAttPlayer->m_Param.GetCharSerial();
    const int raceCode = m_pAttPlayer->m_Param.GetRaceCode();
    CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
    const unsigned __int8 bossType = ranking->GetBossType(raceCode, dwSerial);
    if (bossType)
    {
      if (bossType == 2 || bossType == 6)
      {
        normalAttack *= 1.2f;
      }
    }
    else
    {
      normalAttack *= 1.3f;
    }
  }

  const int attackType = forceField->m_nEffectGroup;
  if (attackType >= 0 && attackType <= 2)
  {
    if (m_pp->pDst)
    {
      m_DamList[0].m_pChar = m_pp->pDst;
      m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(static_cast<int>(normalAttack),
        m_pp->nPart,
        m_pp->nTol,
        m_pp->pDst,
        m_pp->bBackAttack);
      m_nDamagedObjNum = 1;
    }
    CalcAvgDamage();
    return;
  }

  if (attackType == 5)
  {
    const int index = forceField->m_nLv;
    FlashDamageProc(s_nLimitDist[index], static_cast<int>(normalAttack), s_nLimitAngle[1][index], 0, false);
    CalcAvgDamage();
    return;
  }

  if (attackType == 4 || attackType == 6)
  {
    const int index = forceField->m_nLv;
    AreaDamageProc(s_nLimitRadius[index], static_cast<int>(normalAttack), m_pp->fArea, 0, false);
    CalcAvgDamage();
  }
}





