#include "pch.h"

#include "CPlayerAttack.h"
#include "CHolyStoneSystem.h"
#include "CLogFile.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CPlayer.h"
#include "GlobalObjects.h"

CPlayerAttack::CPlayerAttack(CPlayer *pThis)
  : CAttack(pThis),
    m_pAttPlayer(pThis)
{
}

__int64 CPlayerAttack::_CalcSkillAttPnt(bool bUseEffBullet)
{
  _base_fld *skillRecord = m_pp->pFld;
  const float levelFactor =
    static_cast<float>(m_pp->nLevel) + (static_cast<float>(7.0f - static_cast<float>(m_pp->nLevel)) * 0.5f);
  const float fConst = *reinterpret_cast<float *>(&skillRecord[11].m_strCode[56]);
  const int lvConst = *reinterpret_cast<int *>(&skillRecord[11].m_strCode[4 * m_pp->nLevel + 24]);

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
    CLogFile::Write(
      &g_Main.m_logSystemError,
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

  int minSel = static_cast<int>(static_cast<float>(m_pp->nMinSel) - _effect_parameter::GetEff_Plus(&m_pAttChar->m_EP, 14));
  if (m_pp->pDst && m_pp->pDst != m_pAttChar)
  {
    minSel = static_cast<int>(static_cast<float>(minSel) + _effect_parameter::GetEff_Plus(&m_pp->pDst->m_EP, 37));
    minSel += MonsterCritical_Exception_Rate(reinterpret_cast<CMonster *>(m_pp->pDst), m_pp->bBackAttack);
  }
  if (minSel < 0)
  {
    minSel = 0;
  }

  int maxSel =
    static_cast<int>(static_cast<float>(m_pp->nMaxSel + m_pp->nMinSel) - _effect_parameter::GetEff_Plus(&m_pAttChar->m_EP, 14));
  if (m_pp->pDst && m_pp->pDst != m_pAttChar)
  {
    maxSel = static_cast<int>(static_cast<float>(maxSel) + _effect_parameter::GetEff_Plus(&m_pp->pDst->m_EP, 37));
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
  _base_fld *skillRecord = m_pp->pFld;
  bool canHit = true;
  CCharacter::BreakStealth(m_pAttChar);

  int attackType = 0;
  if (m_pp->byEffectCode)
  {
    attackType = *reinterpret_cast<int *>(skillRecord[11].m_strCode);
  }
  else
  {
    attackType = *reinterpret_cast<int *>(&skillRecord[11].m_strCode[4 * m_pp->nLevel - 4]);
  }

  if (m_pp->pDst)
  {
    bool isAvoided = false;
    if (_effect_parameter::GetEff_State(&m_pp->pDst->m_EP, 14))
    {
      isAvoided = true;
    }
    else if (_effect_parameter::GetEff_Plus(&m_pp->pDst->m_EP, 27) > 0.0f)
    {
      const float roll = static_cast<float>(rand() % 100);
      if (_effect_parameter::GetEff_Plus(&m_pp->pDst->m_EP, 27) > roll)
      {
        isAvoided = true;
      }
    }
    if (isAvoided)
    {
      if (!m_pp->bPassCount && !m_pp->nClass && !m_pp->pDst->GetWeaponClass(m_pp->pDst))
      {
        const float targetRange = m_pp->pDst->GetAttackRange(m_pp->pDst);
        const float attackerWidth = m_pAttChar->GetWidth(m_pAttChar);
        const float rangeLimit =
          targetRange + (attackerWidth / 2.0f) + _effect_parameter::GetEff_Plus(&m_pp->pDst->m_EP, 4);
        const float dist = GetSqrt(m_pp->pDst->m_fCurPos, m_pAttChar->m_fCurPos);
        if (rangeLimit >= dist)
        {
          m_DamList[0].m_pChar = m_pp->pDst;
          m_DamList[0].m_nDamage = -1;
          m_nDamagedObjNum = 1;
          CCharacter::SendMsg_AttackActEffect(m_pAttChar, 0, m_pp->pDst);
          return;
        }
      }
      if (_effect_parameter::GetEff_Plus(&m_pp->pDst->m_EP, 27) > 0.0f)
      {
        m_DamList[0].m_pChar = m_pp->pDst;
        m_DamList[0].m_nDamage = 0;
        m_nDamagedObjNum = 1;
        return;
      }
    }

    if (_effect_parameter::GetEff_State(&m_pp->pDst->m_EP, 8))
    {
      canHit = false;
    }
    else
    {
      const float baseChance = _effect_parameter::GetEff_Plus(&m_pAttChar->m_EP, 30) + 100.0f;
      const int avoid = m_pp->pDst->GetAvoidRate(m_pp->pDst);
      int chance = static_cast<int>(baseChance - static_cast<float>(avoid));
      if (!m_pAttChar->m_ObjID.m_byID)
      {
        const float add = _effect_parameter::GetEff_Plus(&m_pAttChar->m_EP, 40);
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
    && (CHolyStoneSystem::GetDestroyerSerial(&g_HolySys) == m_pAttChar->m_dwObjSerial
      || CPlayer::IsLastAttBuff(reinterpret_cast<CPlayer *>(m_pAttChar))))
  {
    normalAttack = static_cast<int>(static_cast<float>(normalAttack) * 1.3f);
    effAttack = static_cast<int>(static_cast<float>(effAttack) * 1.3f);
  }

  if (!m_pAttPlayer->m_bInGuildBattle)
  {
    const unsigned int dwSerial = CPlayerDB::GetCharSerial(&m_pAttPlayer->m_Param);
    const int raceCode = CPlayerDB::GetRaceCode(&m_pAttPlayer->m_Param);
    CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
    const unsigned __int8 bossType = CPvpUserAndGuildRankingSystem::GetBossType(ranking, raceCode, dwSerial);
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
      static_cast<int>(static_cast<float>(normalAttack) * _effect_parameter::GetEff_Rate(&m_pAttChar->m_EP, 29));
    effAttack =
      static_cast<int>(static_cast<float>(effAttack) * _effect_parameter::GetEff_Rate(&m_pAttChar->m_EP, 29));
  }
  else
  {
    normalAttack =
      static_cast<int>(
        static_cast<float>(normalAttack) * _effect_parameter::GetEff_Rate(&m_pAttChar->m_EP, m_pp->nClass + 2));
    effAttack =
      static_cast<int>(
        static_cast<float>(effAttack) * _effect_parameter::GetEff_Rate(&m_pAttChar->m_EP, m_pp->nClass + 2));
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
          m_DamList[0].m_nDamage = CCharacter::GetAttackDamPoint(
            m_pAttChar,
            effAttack,
            m_pp->nPart,
            m_pp->nTol,
            m_pp->pDst,
            m_pp->bBackAttack);
        }
        else
        {
          m_DamList[0].m_nDamage = CCharacter::GetAttackDamPoint(
            m_pAttChar,
            normalAttack,
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
        s_nLimitRadius[*reinterpret_cast<int *>(&skillRecord[4].m_strCode[60])],
        normalAttack,
        m_pp->fArea,
        effAttack,
        bUseEffBullet);
      CalcAvgDamage();
      break;
    case 5:
    {
      const int index = *reinterpret_cast<int *>(&skillRecord[4].m_strCode[60]);
      FlashDamageProc(s_nLimitDist[index], normalAttack, s_nLimitAngle[0][index], effAttack, bUseEffBullet);
      CalcAvgDamage();
      break;
    }
    case 7:
      SectorDamageProc(
        *reinterpret_cast<int *>(&skillRecord[4].m_strCode[60]),
        normalAttack,
        s_nLimitAngle[0][*reinterpret_cast<int *>(&skillRecord[4].m_strCode[60])],
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
  _base_fld *weaponField = m_pp->pFld;
  bool canHit = true;
  CCharacter::BreakStealth(m_pAttChar);

  if (m_pp->pDst)
  {
    const int effectGroup = weaponField[4].m_dwIndex;
    if (effectGroup != 4)
    {
      bool isAvoided = false;
      if (_effect_parameter::GetEff_State(&m_pp->pDst->m_EP, 14))
      {
        isAvoided = true;
      }
      else if (_effect_parameter::GetEff_Plus(&m_pp->pDst->m_EP, 27) > 0.0f)
      {
        const float roll = static_cast<float>(rand() % 100);
        if (_effect_parameter::GetEff_Plus(&m_pp->pDst->m_EP, 27) > roll)
        {
          isAvoided = true;
        }
      }
      if (isAvoided)
      {
        if (!m_pp->bPassCount && !m_pp->nClass && !m_pp->pDst->GetWeaponClass(m_pp->pDst))
        {
          const float targetRange = m_pp->pDst->GetAttackRange(m_pp->pDst);
          const float attackerWidth = m_pAttChar->GetWidth(m_pAttChar);
          const float rangeLimit =
            targetRange + (attackerWidth / 2.0f) + _effect_parameter::GetEff_Plus(&m_pp->pDst->m_EP, 4);
          const float dist = GetSqrt(m_pp->pDst->m_fCurPos, m_pAttChar->m_fCurPos);
          if (rangeLimit >= dist)
          {
            m_DamList[0].m_pChar = m_pp->pDst;
            m_DamList[0].m_nDamage = -1;
            m_nDamagedObjNum = 1;
            CCharacter::SendMsg_AttackActEffect(m_pAttChar, 0, m_pp->pDst);
            return;
          }
        }
        if (_effect_parameter::GetEff_Plus(&m_pp->pDst->m_EP, 27) > 0.0f)
        {
          m_DamList[0].m_pChar = m_pp->pDst;
          m_DamList[0].m_nDamage = 0;
          m_nDamagedObjNum = 1;
          return;
        }
      }

      if (_effect_parameter::GetEff_State(&m_pp->pDst->m_EP, 8))
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
      static_cast<int>(static_cast<float>(normalAttack) * _effect_parameter::GetEff_Rate(&m_pAttChar->m_EP, 21));

    if (!m_pAttPlayer->m_bInGuildBattle)
    {
      const unsigned int dwSerial = CPlayerDB::GetCharSerial(&m_pAttPlayer->m_Param);
      const int raceCode = CPlayerDB::GetRaceCode(&m_pAttPlayer->m_Param);
      CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
      const unsigned __int8 bossType = CPvpUserAndGuildRankingSystem::GetBossType(ranking, raceCode, dwSerial);
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
    const float addDef = defAvg * (_effect_parameter::GetEff_Rate(&m_pAttPlayer->m_EP, 44) - 1.0f);

    const float attackFcA = GetAttackFC(m_pAttPlayer, 2u, true, true);
    const float addAttackA = attackFcA * (_effect_parameter::GetEff_Rate(&m_pAttPlayer->m_EP, 43) - 1.0f);

    const float attackFcB0 = GetAttackFC(m_pAttPlayer, 0, true, true);
    const float attackFcB1 = GetAttackFC(m_pAttPlayer, 1u, true, true);
    const float addAttackB =
      (attackFcB0 + attackFcB1) * (_effect_parameter::GetEff_Rate(&m_pAttPlayer->m_EP, 41) - 1.0f);

    const float attackFcC0 = GetAttackFC(m_pAttPlayer, 0, false, true);
    const float attackFcC1 = GetAttackFC(m_pAttPlayer, 1u, false, true);
    const float addAttackC =
      (attackFcC0 + attackFcC1) * (_effect_parameter::GetEff_Rate(&m_pAttPlayer->m_EP, 42) - 1.0f);

    normalAttack =
      static_cast<int>(static_cast<float>(normalAttack) + addAttackA + addDef + addAttackB + addAttackC);

    const int effectGroup = weaponField[4].m_dwIndex;
    if (effectGroup >= 0)
    {
      if (effectGroup <= 3)
      {
        if (m_pp->pDst)
        {
          m_DamList[0].m_pChar = m_pp->pDst;
          m_DamList[0].m_nDamage = CCharacter::GetAttackDamPoint(
            m_pAttChar,
            normalAttack,
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
  _base_fld *skillRecord = m_pp->pFld;
  CCharacter::BreakStealth(m_pAttChar);

  int attackType = 0;
  if (m_pp->byEffectCode)
  {
    attackType = *reinterpret_cast<int *>(skillRecord[11].m_strCode);
  }
  else
  {
    attackType = *reinterpret_cast<int *>(&skillRecord[11].m_strCode[4 * m_pp->nLevel - 4]);
  }

  int normalAttack = m_pp->nAddAttPnt + static_cast<int>(_CalcSkillAttPnt(false));
  if (m_pp->nWpType == 7)
  {
    normalAttack =
      static_cast<int>(static_cast<float>(normalAttack) * _effect_parameter::GetEff_Rate(&m_pAttChar->m_EP, 29));
  }
  else
  {
    normalAttack =
      static_cast<int>(
        static_cast<float>(normalAttack) * _effect_parameter::GetEff_Rate(&m_pAttChar->m_EP, m_pp->nClass + 2));
  }

  if (!m_pAttPlayer->m_bInGuildBattle)
  {
    const unsigned int dwSerial = CPlayerDB::GetCharSerial(&m_pAttPlayer->m_Param);
    const int raceCode = CPlayerDB::GetRaceCode(&m_pAttPlayer->m_Param);
    CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
    const unsigned __int8 bossType = CPvpUserAndGuildRankingSystem::GetBossType(ranking, raceCode, dwSerial);
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
        m_DamList[0].m_nDamage = CCharacter::GetAttackDamPoint(
          m_pAttChar,
          normalAttack,
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
        s_nLimitRadius[*reinterpret_cast<int *>(&skillRecord[4].m_strCode[60])],
        normalAttack,
        m_pp->fArea,
        0,
        false);
      CalcAvgDamage();
      break;
    case 5:
    {
      const int index = *reinterpret_cast<int *>(&skillRecord[4].m_strCode[60]);
      FlashDamageProc(s_nLimitDist[index], normalAttack, s_nLimitAngle[0][index], 0, false);
      CalcAvgDamage();
      break;
    }
    case 7:
      SectorDamageProc(
        *reinterpret_cast<int *>(&skillRecord[4].m_strCode[60]),
        normalAttack,
        s_nLimitAngle[0][*reinterpret_cast<int *>(&skillRecord[4].m_strCode[60])],
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
  _base_fld *forceField = m_pp->pFld;

  float normalAttack = static_cast<float>(m_pp->nAddAttPnt + _CalcForceAttPnt(false));
  normalAttack *= _effect_parameter::GetEff_Rate(&m_pAttChar->m_EP, 4);

  if (!m_pAttPlayer->m_bInGuildBattle)
  {
    const unsigned int dwSerial = CPlayerDB::GetCharSerial(&m_pAttPlayer->m_Param);
    const int raceCode = CPlayerDB::GetRaceCode(&m_pAttPlayer->m_Param);
    CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
    const unsigned __int8 bossType = CPvpUserAndGuildRankingSystem::GetBossType(ranking, raceCode, dwSerial);
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

  const int attackType = *reinterpret_cast<int *>(&forceField[11].m_strCode[4]);
  if (attackType >= 0 && attackType <= 2)
  {
    if (m_pp->pDst)
    {
      m_DamList[0].m_pChar = m_pp->pDst;
      m_DamList[0].m_nDamage = CCharacter::GetAttackDamPoint(
        m_pAttChar,
        static_cast<int>(normalAttack),
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
    const int index = *reinterpret_cast<int *>(&forceField[4].m_strCode[60]);
    FlashDamageProc(s_nLimitDist[index], static_cast<int>(normalAttack), s_nLimitAngle[1][index], 0, false);
    CalcAvgDamage();
    return;
  }

  if (attackType == 4 || attackType == 6)
  {
    const int index = *reinterpret_cast<int *>(&forceField[4].m_strCode[60]);
    AreaDamageProc(s_nLimitRadius[index], static_cast<int>(normalAttack), m_pp->fArea, 0, false);
    CalcAvgDamage();
  }
}
