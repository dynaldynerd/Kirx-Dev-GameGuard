#include "pch.h"

#include "CAttack.h"
#include "CMainThread.h"

#include "CAnimus.h"
#include "CCharacter.h"
#include "CGameObject.h"
#include "CGuardTower.h"
#include "CHolyStoneSystem.h"
#include "CLogFile.h"
#include "CMapData.h"
#include "CMonster.h"
#include "CObjectList.h"
#include "CPlayer.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "pnt_rect.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

CRecordData *CAttack::s_pSkillData = nullptr;
_attack_param CAttack::s_DefParam = {};

float fR = 0.0f;
float fRLf = 0.0f;
float fRMf = 0.0f;
float fR_0 = 0.0f;
float fRLf_0 = 0.0f;
float fRMf_0 = 0.0f;
float fR_1 = 0.0f;
float fRLf_1 = 0.0f;
float fRMf_1 = 0.0f;

int s_nLimitDist[8] = {};
int s_nLimitRadius[8] = {};
int s_nLimitAngle[2][8] = {};
int s_nLimitRadius_0[16] = {};
int s_nLimitAngle_0 = 0;
int s_nLimitAngle_1 = 0;

int s_Mon_nLimitDist[8] = {};
int s_Mon_nLimitRadius[8] = {};
int s_Mon_nLimitAngle[2][8] = {};

_attack_param::_attack_param()
{
  memset_0(this, 0, sizeof(_attack_param));
}

SKILL::SKILL()
{
  m_Type = -1;
  m_Element = -1;
  m_MinDmg = 0;
  m_StdDmg = 0;
  m_MaxDmg = 0;
  m_CritDmg = 0;
  m_MinProb = 0;
  m_MaxProb = 0;
  m_IsCritical = 0;
  m_Len = 0;
  m_CastDelay = 0;
  m_Delay = 0;
  m_bLoad = 0;
  m_Active = 0;
  m_BefTime = 0;
}

CAttack::CAttack(CCharacter *pThis)
  : m_pp(&s_DefParam),
    m_pAttChar(pThis),
    m_bIsCrtAtt(false),
    m_nDamagedObjNum(0),
    m_bFailure(false)
{
  std::memset(m_DamList, 0, sizeof(m_DamList));
}

void CAttack::SetStaticMember(CRecordData *effectTable)
{
  s_pSkillData = effectTable;
}

void CAttack::SetActiveSucc(bool bSucc)
{
  m_bActiveSucc = bSucc;
}

void CAttack::AttackGen(_attack_param *pParam, bool bMustMiss, bool bUseEffBullet)
{
  m_nDamagedObjNum = 0;
  m_bIsCrtAtt = false;
  m_pp = pParam;
  bool canHit = true;
  m_nDamagedObjNum = 0;
  m_pAttChar->BreakStealth();

  if (m_pp->pDst)
  {
    bool isAvoided = false;
    if (m_pp->pDst->m_EP.GetEff_State(14))
    {
      isAvoided = true;
    }
    else if (m_pp->pDst->m_EP.GetEff_Plus(27) > 0.0f)
    {
      const float roll = static_cast<float>(rand() % 100);
      if (m_pp->pDst->m_EP.GetEff_Plus(27) > roll)
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
          targetRange + (attackerWidth / 2.0f) + m_pp->pDst->m_EP.GetEff_Plus(4);
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

    if (canHit && bMustMiss)
    {
      canHit = false;
    }

    if (!canHit)
    {
      m_DamList[0].m_pChar = m_pp->pDst;
      m_DamList[0].m_nDamage = 0;
      m_nDamagedObjNum = 1;
      return;
    }
  }

  float normalAttack = static_cast<float>(m_pp->nAddAttPnt + _CalcGenAttPnt(false));
  float effAttack = static_cast<float>(m_pp->nAddAttPnt + _CalcGenAttPnt(bUseEffBullet));

  if (!m_pAttChar->m_ObjID.m_byID
    && (g_HolySys.GetDestroyerSerial() == m_pAttChar->m_dwObjSerial
      || reinterpret_cast<CPlayer *>(m_pAttChar)->IsLastAttBuff()))
  {
    normalAttack *= 1.3f;
    effAttack *= 1.3f;
  }

  if (!m_pAttChar->m_ObjID.m_byID)
  {
    auto *attacker = reinterpret_cast<CPlayer *>(m_pAttChar);
    if (!attacker->m_bInGuildBattle)
    {
      const unsigned int dwSerial = attacker->m_Param.GetCharSerial();
      const int raceCode = attacker->m_Param.GetRaceCode();
      CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
      const unsigned __int8 bossType = ranking->GetBossType(static_cast<unsigned __int8>(raceCode), dwSerial);
      if (bossType)
      {
        if (bossType == 2 || bossType == 6)
        {
          normalAttack *= 1.2f;
          effAttack *= 1.2f;
        }
      }
      else
      {
        normalAttack *= 1.3f;
        effAttack *= 1.3f;
      }
    }
  }

  if (m_pAttChar->m_ObjID.m_byID == 3)
  {
    auto *tower = reinterpret_cast<CGuardTower *>(m_pAttChar);
    if (tower && tower->m_pMasterTwr)
    {
      const float attackFc0 = GetAttackFC(tower->m_pMasterTwr, 2u, true, false);
      const float effRate0 = tower->m_pMasterTwr->m_EP.GetEff_Rate(57);
      const float add0 = attackFc0 * (effRate0 - 1.0f);

      const float attackFc1 = GetAttackFC(tower->m_pMasterTwr, 0, true, false);
      const float effRate1 = tower->m_pMasterTwr->m_EP.GetEff_Rate(58);
      const float add1 = attackFc1 * (effRate1 - 1.0f);

      const float attackFc2 = GetAttackFC(tower->m_pMasterTwr, 0, false, false);
      const float effRate2 = tower->m_pMasterTwr->m_EP.GetEff_Rate(59);
      const float add2 = attackFc2 * (effRate2 - 1.0f);

      const float attackFc3 = GetAttackFC(tower->m_pMasterTwr, 1u, true, false);
      const float effRate3 = tower->m_pMasterTwr->m_EP.GetEff_Rate(60);
      const float add3 = attackFc3 * (effRate3 - 1.0f);

      const float attackFc4 = GetAttackFC(tower->m_pMasterTwr, 1u, false, false);
      const float effRate4 = tower->m_pMasterTwr->m_EP.GetEff_Rate(61);
      const float add4 = attackFc4 * (effRate4 - 1.0f);

      normalAttack = (((normalAttack + add0) + add1) + add2) + add3 + add4;
      effAttack = (((effAttack + add0) + add1) + add2) + add3 + add4;
    }
  }

  if (m_pp->nWpType == 7)
  {
    const float rate = m_pAttChar->m_EP.GetEff_Rate(29);
    normalAttack *= rate;
    effAttack *= rate;
  }
  else
  {
    const float rate = m_pAttChar->m_EP.GetEff_Rate(m_pp->nClass);
    normalAttack *= rate;
    effAttack *= rate;
  }

  const int attackType = pParam->nAttactType;
  if (attackType >= 0 && attackType <= 2)
  {
    m_DamList[0].m_pChar = m_pp->pDst;
    if (bUseEffBullet)
    {
      m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(
        static_cast<int>(effAttack),
        m_pp->nPart,
        m_pp->nTol,
        m_pp->pDst,
        m_pp->bBackAttack);
    }
    else
    {
      m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(
        static_cast<int>(normalAttack),
        m_pp->nPart,
        m_pp->nTol,
        m_pp->pDst,
        m_pp->bBackAttack);
    }
    m_nDamagedObjNum = 1;
  }
  else if (attackType == 5)
  {
    if (m_pp->nExtentRange > 0)
    {
      FlashDamageProc(m_pp->nExtentRange, static_cast<int>(normalAttack), 90, static_cast<int>(effAttack), bUseEffBullet);
    }
  }
  else if (attackType == 6)
  {
    if (m_pp->nExtentRange > 0)
    {
      AreaDamageProc(
        m_pp->nExtentRange,
        static_cast<int>(normalAttack),
        m_pp->fArea,
        static_cast<int>(effAttack),
        bUseEffBullet);
    }
  }
  else
  {
    m_DamList[0].m_pChar = m_pp->pDst;
    m_DamList[0].m_nDamage = 0;
    m_nDamagedObjNum = 1;
  }

  CalcAvgDamage();
}

void CAttack::AttackForce(_attack_param *pParam, bool bUseEffBullet)
{
  m_nDamagedObjNum = 0;
  m_bIsCrtAtt = false;
  m_pp = pParam;
  _base_fld *forceField = m_pp->pFld;
  bool canHit = true;
  m_pAttChar->BreakStealth();

  if (m_pp->pDst)
  {
    if (m_pp->pDst->m_EP.GetEff_State(8))
    {
      canHit = false;
    }
    else
    {
      const float baseChance = m_pAttChar->m_EP.GetEff_Plus(31) + 100.0f;
      const int avoid = m_pp->pDst->GetAvoidRate();
      int chance = static_cast<int>(baseChance - static_cast<float>(avoid));
      if (!m_pAttChar->m_ObjID.m_byID)
      {
        const float add = m_pAttChar->m_EP.GetEff_Plus(40);
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
  }

  if (!canHit)
  {
    if (m_pp->pDst)
    {
      m_DamList[0].m_pChar = m_pp->pDst;
      m_DamList[0].m_nDamage = 0;
      m_nDamagedObjNum = 1;
    }
    return;
  }

  float normalAttack = static_cast<float>(m_pp->nAddAttPnt + _CalcForceAttPnt(false));
  normalAttack *= m_pAttChar->m_EP.GetEff_Rate(4);
  float effAttack = static_cast<float>(m_pp->nAddAttPnt + _CalcForceAttPnt(bUseEffBullet));
  effAttack *= m_pAttChar->m_EP.GetEff_Rate(4);

  if (!m_pAttChar->m_ObjID.m_byID
    && (g_HolySys.GetDestroyerSerial() == m_pAttChar->m_dwObjSerial
      || reinterpret_cast<CPlayer *>(m_pAttChar)->IsLastAttBuff()))
  {
    normalAttack *= 1.3f;
    effAttack *= 1.3f;
  }

  if (!m_pAttChar->m_ObjID.m_byID)
  {
    auto *attacker = reinterpret_cast<CPlayer *>(m_pAttChar);
    if (!attacker->m_bInGuildBattle)
    {
      const unsigned int dwSerial = attacker->m_Param.GetCharSerial();
      const int raceCode = attacker->m_Param.GetRaceCode();
      CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
      const unsigned __int8 bossType = ranking->GetBossType(static_cast<unsigned __int8>(raceCode), dwSerial);
      if (bossType)
      {
        if (bossType == 2 || bossType == 6)
        {
          normalAttack *= 1.2f;
          effAttack *= 1.2f;
        }
      }
      else
      {
        normalAttack *= 1.3f;
        effAttack *= 1.3f;
      }
    }
  }

  const int attackType = *reinterpret_cast<int *>(&forceField[11].m_strCode[4]);
  if (attackType >= 0 && attackType <= 2)
  {
    if (m_pp->pDst)
    {
      m_DamList[0].m_pChar = m_pp->pDst;
      if (bUseEffBullet)
      {
        m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(
          static_cast<int>(effAttack),
          m_pp->nPart,
          m_pp->nTol,
          m_pp->pDst,
          m_pp->bBackAttack);
      }
      else
      {
        m_DamList[0].m_nDamage = m_pAttChar->GetAttackDamPoint(
          static_cast<int>(normalAttack),
          m_pp->nPart,
          m_pp->nTol,
          m_pp->pDst,
          m_pp->bBackAttack);
      }
      m_nDamagedObjNum = 1;
    }
  }
  else if (attackType == 5)
  {
    const int index = *reinterpret_cast<int *>(&forceField[4].m_strCode[60]);
    FlashDamageProc(
      s_nLimitDist[index],
      static_cast<int>(normalAttack),
      s_nLimitAngle[1][index],
      static_cast<int>(effAttack),
      bUseEffBullet);
  }
  else if (attackType == 4 || attackType == 6)
  {
    const int index = *reinterpret_cast<int *>(&forceField[4].m_strCode[60]);
    AreaDamageProc(
      s_nLimitRadius[index],
      static_cast<int>(normalAttack),
      m_pp->fArea,
      static_cast<int>(effAttack),
      bUseEffBullet);
  }
  else
  {
    return;
  }

  CalcAvgDamage();
}

float CAttack::GetAttackFC(CPlayer *pPlayer, unsigned __int8 bySkill, bool bNear, bool bUnit)
{
  _STORAGE_LIST::_db_con *weaponItem = pPlayer->m_Param.m_dbEquip.m_pStorageList + 6;
  if (!weaponItem->m_bLoad)
  {
    return 0.0f;
  }

  _base_fld *record = g_Main.m_tblItemData[6].GetRecord(weaponItem->m_wItemIndex);
  if (!record)
  {
    return 0.0f;
  }

  const int mastery = pPlayer->m_pmMst.GetMasteryPerMast(0, pPlayer->m_pmWpn.byWpClass);
  float value = 0.0f;

  if (bySkill)
  {
    if (bySkill == 1)
    {
      if (bUnit)
      {
        value = (*reinterpret_cast<float *>(record[10].m_strCode) * pPlayer->m_EP.GetEff_Rate(32))
          + static_cast<float>(mastery);
      }
      else
      {
        value = (static_cast<float>(pPlayer->m_pmWpn.nGaMaxAF) * pPlayer->m_EP.GetEff_Rate(32))
          + static_cast<float>(mastery);
      }
      if (!bNear || GetWeaponClass(weaponItem->m_wItemIndex))
      {
        if (bNear || GetWeaponClass(weaponItem->m_wItemIndex) != 1)
        {
          return 0.0f;
        }
        return value * pPlayer->m_EP.GetEff_Rate(3);
      }
      return value * pPlayer->m_EP.GetEff_Rate(2);
    }
    if (bUnit)
    {
      return (*reinterpret_cast<float *>(&record[10].m_strCode[24]) * pPlayer->m_EP.GetEff_Rate(32))
        + static_cast<float>(pPlayer->m_pmMst.m_mtyStaff);
    }
    return (static_cast<float>(pPlayer->m_pmWpn.nMaMaxAF) * pPlayer->m_EP.GetEff_Rate(32))
      + static_cast<float>(pPlayer->m_pmMst.m_mtyStaff);
  }

  if (bUnit)
  {
    value = (*reinterpret_cast<float *>(record[10].m_strCode) * pPlayer->m_EP.GetEff_Rate(32))
      + static_cast<float>(CPlayer::s_nAddMstFc[mastery]);
  }
  else
  {
    value = (static_cast<float>(pPlayer->m_pmWpn.nGaMaxAF) * pPlayer->m_EP.GetEff_Rate(32))
      + static_cast<float>(CPlayer::s_nAddMstFc[mastery]);
  }

  if (!bNear || GetWeaponClass(weaponItem->m_wItemIndex))
  {
    if (bNear || GetWeaponClass(weaponItem->m_wItemIndex) != 1)
    {
      return 0.0f;
    }
    return value * pPlayer->m_EP.GetEff_Rate(1);
  }

  return value * pPlayer->m_EP.GetEff_Rate(0);
}

__int64 CAttack::GetMeleeSkillIndex(int nMeleeTechCode)
{
  const int recordCount = static_cast<int>(s_pSkillData->GetRecordNum());
  for (int index = 0; index < recordCount; ++index)
  {
    _base_fld *record = s_pSkillData->GetRecord(index);
    if (!record[1].m_dwIndex && *reinterpret_cast<int *>(&record[16].m_strCode[52]) == nMeleeTechCode)
    {
      return index;
    }
  }
  return 0xFFFFFFFFLL;
}

__int64 CAttack::_CalcGenAttPnt(bool bUseEffBullet)
{
  unsigned int maxAttack = 0;
  int midAttack = 0;
  if (bUseEffBullet)
  {
    maxAttack = static_cast<unsigned int>(
      static_cast<int>(((m_pp->nMaxAFPlus * (m_pp->nMaxAFPlus + 125.0f)) / (m_pp->nMaxAFPlus + 50.0f)) + 0.5f));
    midAttack = static_cast<int>(((m_pp->nMaxAFPlus + m_pp->nMinAFPlus) / 2.0f) + 0.5f);
  }
  else
  {
    maxAttack = static_cast<unsigned int>(
      static_cast<int>(((m_pp->nMaxAF * (m_pp->nMaxAF + 125.0f)) / (m_pp->nMaxAF + 50.0f)) + 0.5f));
    midAttack = static_cast<int>(((m_pp->nMaxAF + m_pp->nMinAF) / 2.0f) + 0.5f);
  }

  if (m_pAttChar->m_EP.GetEff_State(9))
  {
    return maxAttack;
  }
  if (m_pp->nMaxAttackPnt > 0)
  {
    return maxAttack;
  }

  if (m_pp->nMaxAttackPnt >= 0)
  {
    const int randValue = m_pAttChar->m_rtPer100.GetRand();
    int minSel = static_cast<int>(static_cast<float>(m_pp->nMinSel) - m_pAttChar->m_EP.GetEff_Plus(14));
    if (m_pp->pDst && m_pp->pDst != m_pAttChar)
    {
      minSel = static_cast<int>(static_cast<float>(minSel) + m_pp->pDst->m_EP.GetEff_Plus(37));
      minSel += static_cast<int>(MonsterCritical_Exception_Rate(reinterpret_cast<CMonster *>(m_pp->pDst), m_pp->bBackAttack));
    }
    if (minSel < 0)
    {
      minSel = 0;
    }

    int maxSel = static_cast<int>(
      static_cast<float>(m_pp->nMaxSel + m_pp->nMinSel) - m_pAttChar->m_EP.GetEff_Plus(14));
    if (m_pp->pDst && m_pp->pDst != m_pAttChar)
    {
      maxSel = static_cast<int>(static_cast<float>(maxSel) + m_pp->pDst->m_EP.GetEff_Plus(37));
      maxSel += static_cast<int>(MonsterCritical_Exception_Rate(reinterpret_cast<CMonster *>(m_pp->pDst), m_pp->bBackAttack));
    }
    if (maxSel < 0)
    {
      maxSel = 0;
    }

    if (randValue > minSel)
    {
      if (randValue > maxSel)
      {
        m_bIsCrtAtt = true;
        return maxAttack;
      }

      if (bUseEffBullet)
      {
        if ((m_pp->nMaxAFPlus - midAttack) <= 0)
        {
          return midAttack;
        }
        return rand() % (m_pp->nMaxAFPlus - midAttack) + midAttack;
      }

      if ((m_pp->nMaxAF - midAttack) <= 0)
      {
        return midAttack;
      }
      return rand() % (m_pp->nMaxAF - midAttack) + midAttack;
    }

    if (bUseEffBullet)
    {
      if ((midAttack - m_pp->nMinAFPlus) <= 0)
      {
        return static_cast<unsigned int>(m_pp->nMinAFPlus);
      }
      return static_cast<unsigned int>(m_pp->nMinAFPlus + rand() % (midAttack - m_pp->nMinAFPlus));
    }

    if ((midAttack - m_pp->nMinAF) <= 0)
    {
      return static_cast<unsigned int>(m_pp->nMinAF);
    }
    return static_cast<unsigned int>(m_pp->nMinAF + rand() % (midAttack - m_pp->nMinAF));
  }

  if (bUseEffBullet)
  {
    return static_cast<unsigned int>(m_pp->nMinAFPlus);
  }
  return static_cast<unsigned int>(m_pp->nMinAF);
}

__int64 CAttack::_CalcForceAttPnt(bool bUseEffBullet)
{
  _base_fld *skillField = m_pp->pFld;
  const float levelFactor = static_cast<float>(m_pp->nLevel) + ((7.0f - static_cast<float>(m_pp->nLevel)) * 0.5f);
  const float skillConst = *reinterpret_cast<float *>(&skillField[11].m_strCode[8]);

  unsigned int minAttack = 0;
  int maxAttack = 0;
  if (bUseEffBullet)
  {
    minAttack = static_cast<unsigned int>(static_cast<int>(
      ((m_pp->nMinAFPlus
        * (fR_0 + ((levelFactor / 7.0f) * fRLf_0)
           + ((static_cast<float>(m_pp->nMastery) / 99.0f) * fRMf_0)))
       * skillConst)
      + 0.5f));
    maxAttack = static_cast<int>(
      ((m_pp->nMaxAFPlus
        * (fR_0 + ((levelFactor / 7.0f) * fRLf_0)
           + ((static_cast<float>(m_pp->nMastery) / 99.0f) * fRMf_0)))
       * skillConst)
      + 0.5f);
  }
  else
  {
    minAttack = static_cast<unsigned int>(static_cast<int>(
      ((m_pp->nMinAF
        * (fR_0 + ((levelFactor / 7.0f) * fRLf_0)
           + ((static_cast<float>(m_pp->nMastery) / 99.0f) * fRMf_0)))
       * skillConst)
      + 0.5f));
    maxAttack = static_cast<int>(
      ((m_pp->nMaxAF
        * (fR_0 + ((levelFactor / 7.0f) * fRLf_0)
           + ((static_cast<float>(m_pp->nMastery) / 99.0f) * fRMf_0)))
       * skillConst)
      + 0.5f);
  }

  const unsigned int criticalAttack = static_cast<unsigned int>(
    static_cast<int>((static_cast<double>((maxAttack + 125) / (maxAttack + 50) * maxAttack)) + 0.5));

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
    minSel += static_cast<int>(MonsterCritical_Exception_Rate(reinterpret_cast<CMonster *>(m_pp->pDst), m_pp->bBackAttack));
  }
  if (minSel < 0)
  {
    minSel = 0;
  }

  int maxSel = static_cast<int>(
    static_cast<float>(m_pp->nMaxSel + m_pp->nMinSel) - m_pAttChar->m_EP.GetEff_Plus(14));
  if (m_pp->pDst && m_pp->pDst != m_pAttChar)
  {
    maxSel = static_cast<int>(static_cast<float>(maxSel) + m_pp->pDst->m_EP.GetEff_Plus(37));
    maxSel += static_cast<int>(MonsterCritical_Exception_Rate(reinterpret_cast<CMonster *>(m_pp->pDst), m_pp->bBackAttack));
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
    if ((maxAttack - static_cast<int>(midAttack)) <= 0)
    {
      return midAttack;
    }
    return rand() % (maxAttack - static_cast<int>(midAttack)) + midAttack;
  }

  if ((static_cast<int>(midAttack) - static_cast<int>(minAttack)) <= 0)
  {
    return minAttack;
  }
  return rand() % (static_cast<int>(midAttack) - static_cast<int>(minAttack)) + minAttack;
}

__int64 CAttack::MonsterCritical_Exception_Rate(CMonster *pCharacter, bool bBackAttack)
{
  if (!pCharacter || pCharacter->m_ObjID.m_byID != 1)
  {
    return 0;
  }

  float criticalRate = static_cast<float>(pCharacter->GetCritical_Exception_Rate());
  int outValue = -1;
  if (bBackAttack && pCharacter->GetViewAngleCap(2, &outValue))
  {
    criticalRate = criticalRate * (static_cast<float>(100 - outValue) / 100.0f);
    if (criticalRate < 0.0f)
    {
      criticalRate = 0.0f;
    }
  }
  return static_cast<unsigned int>(static_cast<int>(criticalRate));
}

void CAttack::CalcAvgDamage()
{
  float avgDamage = FLOAT_1_0;
  if (m_nDamagedObjNum == 1)
  {
    int damageValue = 0;
    if (m_DamList[0].m_nDamage == -2)
    {
      damageValue = 2;
    }
    else
    {
      damageValue = m_DamList[0].m_nDamage;
    }
    avgDamage = static_cast<float>(damageValue);
  }
  else if (m_nDamagedObjNum > 1)
  {
    float damageSum = 0.0f;
    for (int i = 0; i < m_nDamagedObjNum; ++i)
    {
      int damageValue = 0;
      if (m_DamList[i].m_nDamage == -2)
      {
        damageValue = 0;
      }
      else
      {
        damageValue = m_DamList[i].m_nDamage;
      }
      damageSum += static_cast<float>(damageValue);
    }
    avgDamage = damageSum / static_cast<float>(m_nDamagedObjNum);
  }

  if (avgDamage <= 1.0f)
  {
    if (m_bIsCrtAtt)
    {
      m_bIsCrtAtt = false;
    }
    m_bFailure = true;
  }
}

void CAttack::FlashDamageProc(
  int nLimDist,
  int nAttPower,
  int nAngle,
  int nEffAttPower,
  bool bUseEffBullet)
{
  if (!m_pp->pDst)
  {
    return;
  }

  m_DamList[0].m_pChar = m_pp->pDst;
  if (m_pp->bMatchless)
  {
    m_DamList[0].m_nDamage = m_pp->pDst->GetHP();
  }
  else
  {
    const int damageValue = bUseEffBullet
      ? m_pAttChar->GetAttackDamPoint(nEffAttPower, m_pp->nPart, m_pp->nTol, m_pp->pDst, m_pp->bBackAttack)
      : m_pAttChar->GetAttackDamPoint(nAttPower, m_pp->nPart, m_pp->nTol, m_pp->pDst, m_pp->bBackAttack);
    m_DamList[0].m_nDamage = damageValue;
  }
  m_nDamagedObjNum = 1;

  if (m_pp->pDst->GetCurSecNum() == -1)
  {
    return;
  }

  if (m_pAttChar->m_pCurMap != m_pp->pDst->m_pCurMap)
  {
    _bsp_info *dstBsp = m_pp->pDst->m_pCurMap->GetBspInfo();
    const char *dstObjName = m_pp->pDst->GetObjName();
    _bsp_info *srcBsp = m_pAttChar->m_pCurMap->GetBspInfo();
    const char *srcObjName = m_pAttChar->GetObjName();
    g_Main.m_logSystemError.Write(
      "FlashDamage Error AttackTarget Map : Attack Obj( %s : %s ) Dst Obj( %s : %s )",
      srcObjName,
      srcBsp->m_szBspName,
      dstObjName,
      dstBsp->m_szBspName);
    return;
  }

  CMapData *targetMap = m_pp->pDst->m_pCurMap;
  _pnt_rect rect{};
  const int curSec = m_pp->pDst->GetCurSecNum();
  targetMap->GetRectInRadius(&rect, 1, curSec);
  bool inGuildBattle = false;

  for (int secY = rect.nStarty; secY <= rect.nEndy; ++secY)
  {
    for (int secX = rect.nStartx; secX <= rect.nEndx; ++secX)
    {
      _sec_info *secInfo = targetMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * secY + secX;
      CObjectList *sectorList = m_pAttChar->m_pCurMap->GetSectorListObj(m_pAttChar->m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      for (_object_list_point *node = sectorList->m_Head.m_pNext; node != &sectorList->m_Tail; node = node->m_pNext)
      {
        CGameObject *object = node->m_pItem;
        if (m_nDamagedObjNum >= 30)
        {
          return;
        }
        if (object->m_ObjID.m_byKind || object == m_pAttChar || object == m_pp->pDst || !object->m_bLive
            || object->m_bCorpse || !object->IsBeAttackedAble(false))
        {
          continue;
        }

        inGuildBattle = false;
        if (!CheckGuildBattleLimit(object, &inGuildBattle))
        {
          if (inGuildBattle)
          {
            goto CHECK_FLASH_DAMAGE;
          }
          const int objRace = object->GetObjRace();
          const int attRace = m_pAttChar->GetObjRace();
          if (objRace != attRace || m_pAttChar->m_ObjID.m_byID)
          {
            goto CHECK_FLASH_DAMAGE;
          }

          CPlayer *attPlayer = static_cast<CPlayer *>(m_pAttChar);
          if (object->m_ObjID.m_byID || static_cast<CPlayer *>(object)->IsPunished(1u, false)
              || (attPlayer)->IsChaosMode())
          {
            if (object->m_ObjID.m_byID != 11
                || ((static_cast<AutominePersonal *>(object)->GetOwner() != attPlayer)
                    && (attPlayer)->IsChaosMode()))
            {
              goto CHECK_FLASH_DAMAGE;
            }
          }
        }
        continue;

CHECK_FLASH_DAMAGE:
        if (object->IsBeDamagedAble(m_pAttChar))
        {
          CAnimus *recallAnimus = nullptr;
          if (!m_pAttChar->m_ObjID.m_byID)
          {
            recallAnimus = static_cast<CPlayer *>(m_pAttChar)->GetRecallAnimus();
          }
          if (!m_pAttChar->m_ObjID.m_byID || !recallAnimus || recallAnimus != object)
          {
            if (m_pAttChar->m_ObjID.m_byID
                || static_cast<CPlayer *>(m_pAttChar)->GetObjRace() != 1
                || (static_cast<CPlayer *>(m_pAttChar)->IsChaosMode()) || object->m_ObjID.m_byID != 3)
            {
              if (object->m_ObjID.m_byID != 7)
              {
                if (object->m_ObjID.m_byID != 4
                    || m_pAttChar->GetObjRace() != m_pAttChar->GetObjRace()
                    || (static_cast<CPlayer *>(m_pAttChar)->IsChaosMode()))
                {
                  if (m_pAttChar->IsAttackableInTown() || object->IsAttackableInTown()
                      || (!m_pAttChar->IsInTown() && !object->IsInTown()))
                  {
                    const float heightDiff = std::abs(m_pAttChar->m_fCurPos[1] - object->m_fCurPos[1]);
                    if (heightDiff <= 350.0f)
                    {
                      const float distance = GetSqrt(m_pp->pDst->m_fCurPos, object->m_fCurPos);
                      const float width = object->GetWidth();
                      float adjustedDist = 0.0f;
                      if ((distance - (width / 2.0f)) > 0.0f)
                      {
                        const float distance2 = GetSqrt(m_pp->pDst->m_fCurPos, object->m_fCurPos);
                        const float width2 = object->GetWidth();
                        adjustedDist = distance2 - (width2 / 2.0f);
                      }
                      const int distInt = static_cast<int>(adjustedDist);
                      if (distInt < nLimDist)
                      {
                        if (IsCharInSector(
                              object->m_fCurPos,
                              m_pAttChar->m_fCurPos,
                              m_pp->pDst->m_fCurPos,
                              static_cast<float>(nAngle),
                              static_cast<float>(nLimDist)))
                        {
                          m_DamList[m_nDamagedObjNum].m_pChar = static_cast<CCharacter *>(object);
                          if (m_pp->bMatchless)
                          {
                            m_DamList[m_nDamagedObjNum].m_nDamage = object->GetHP();
                            ++m_nDamagedObjNum;
                            continue;
                          }

                          const int attackPart = static_cast<CCharacter *>(object)->GetAttackRandomPart();
                          m_DamList[m_nDamagedObjNum].m_nDamage = m_pAttChar->GetAttackDamPoint(
                            nAttPower,
                            attackPart,
                            m_pp->nTol,
                            static_cast<CCharacter *>(object),
                            false);
                          if (m_DamList[m_nDamagedObjNum].m_nDamage != -2)
                          {
                            m_DamList[m_nDamagedObjNum].m_nDamage = static_cast<int>(
                              static_cast<float>(m_DamList[m_nDamagedObjNum].m_nDamage)
                              * (static_cast<float>(nLimDist - distInt) / static_cast<float>(nLimDist)));
                            if (m_DamList[m_nDamagedObjNum].m_nDamage < 1)
                            {
                              continue;
                            }
                          }
                          ++m_nDamagedObjNum;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

void CAttack::AreaDamageProc(
  int nLimitRadius,
  int nAttPower,
  float *pTar,
  int nEffAttPower,
  bool bUseEffBullet)
{
  CMapData *currentMap = m_pAttChar->m_pCurMap;
  const int secNum = currentMap->GetSectorIndex(pTar);
  if (secNum == -1)
  {
    return;
  }

  if (m_pp->pDst && m_pAttChar != m_pp->pDst)
  {
    m_DamList[0].m_pChar = m_pp->pDst;
    if (m_pp->bMatchless)
    {
      m_DamList[0].m_nDamage = m_pp->pDst->GetHP();
    }
    else
    {
      const int damageValue = bUseEffBullet
        ? m_pAttChar->GetAttackDamPoint(nEffAttPower, m_pp->nPart, m_pp->nTol, m_pp->pDst, m_pp->bBackAttack)
        : m_pAttChar->GetAttackDamPoint(nAttPower, m_pp->nPart, m_pp->nTol, m_pp->pDst, m_pp->bBackAttack);
      m_DamList[0].m_nDamage = damageValue;
    }
    m_nDamagedObjNum = 1;
  }

  _pnt_rect rect{};
  currentMap->GetRectInRadius(&rect, 1, secNum);
  bool inGuildBattle = false;

  for (int secY = rect.nStarty; secY <= rect.nEndy; ++secY)
  {
    for (int secX = rect.nStartx; secX <= rect.nEndx; ++secX)
    {
      _sec_info *secInfo = currentMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * secY + secX;
      CObjectList *sectorList = m_pAttChar->m_pCurMap->GetSectorListObj(m_pAttChar->m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      for (_object_list_point *node = sectorList->m_Head.m_pNext; node != &sectorList->m_Tail; node = node->m_pNext)
      {
        CGameObject *object = node->m_pItem;
        if (m_nDamagedObjNum >= 30)
        {
          return;
        }
        if (object->m_ObjID.m_byKind || object == m_pAttChar || object == m_pp->pDst || !object->m_bLive
            || object->m_bCorpse || !object->IsBeAttackedAble(false))
        {
          continue;
        }

        inGuildBattle = false;
        if (!CheckGuildBattleLimit(object, &inGuildBattle))
        {
          if (inGuildBattle)
          {
            goto CHECK_AREA_DAMAGE;
          }
          const int objRace = object->GetObjRace();
          const int attRace = m_pAttChar->GetObjRace();
          if (objRace != attRace)
          {
            goto CHECK_AREA_DAMAGE;
          }

          if (!m_pAttChar->m_ObjID.m_byID)
          {
            CPlayer *attPlayer = static_cast<CPlayer *>(m_pAttChar);
            if (object->m_ObjID.m_byID || static_cast<CPlayer *>(object)->IsPunished(1u, false)
                || (attPlayer)->IsChaosMode())
            {
              if (object->m_ObjID.m_byID != 11
                  || ((static_cast<AutominePersonal *>(object)->GetOwner() != attPlayer)
                      && (attPlayer)->IsChaosMode()))
              {
                goto CHECK_AREA_DAMAGE;
              }
            }
          }
        }
        continue;

CHECK_AREA_DAMAGE:
        if (object->IsBeDamagedAble(m_pAttChar))
        {
          CAnimus *recallAnimus = nullptr;
          if (!m_pAttChar->m_ObjID.m_byID)
          {
            recallAnimus = static_cast<CPlayer *>(m_pAttChar)->GetRecallAnimus();
          }
          if (!m_pAttChar->m_ObjID.m_byID || !recallAnimus || recallAnimus != object)
          {
            if (m_pAttChar->m_ObjID.m_byID
                || static_cast<CPlayer *>(m_pAttChar)->GetObjRace() != 1
                || (static_cast<CPlayer *>(m_pAttChar)->IsChaosMode()) || object->m_ObjID.m_byID != 3)
            {
              if (object->m_ObjID.m_byID != 7)
              {
                if (object->m_ObjID.m_byID != 4
                    || m_pAttChar->GetObjRace() != m_pAttChar->GetObjRace()
                    || (static_cast<CPlayer *>(m_pAttChar)->IsChaosMode()))
                {
                  if (m_pAttChar->IsAttackableInTown() || object->IsAttackableInTown()
                      || (!m_pAttChar->IsInTown() && !object->IsInTown()))
                  {
                    const float heightDiff = std::abs(m_pAttChar->m_fCurPos[1] - object->m_fCurPos[1]);
                    if (heightDiff <= 350.0f)
                    {
                      const float distance = GetSqrt(pTar, object->m_fCurPos);
                      const float width = object->GetWidth();
                      float adjustedDist = 0.0f;
                      if ((distance - (width / 2.0f)) > 0.0f)
                      {
                        const float distance2 = GetSqrt(pTar, object->m_fCurPos);
                        const float width2 = object->GetWidth();
                        adjustedDist = distance2 - (width2 / 2.0f);
                      }
                      const int distInt = static_cast<int>(adjustedDist);
                      if (distInt <= nLimitRadius)
                      {
                        m_DamList[m_nDamagedObjNum].m_pChar = static_cast<CCharacter *>(object);
                        if (m_pp->bMatchless)
                        {
                          m_DamList[m_nDamagedObjNum].m_nDamage = object->GetHP();
                          ++m_nDamagedObjNum;
                          continue;
                        }

                        const int attackPart = static_cast<CCharacter *>(object)->GetAttackRandomPart();
                        m_DamList[m_nDamagedObjNum].m_nDamage = m_pAttChar->GetAttackDamPoint(
                          nAttPower,
                          attackPart,
                          m_pp->nTol,
                          static_cast<CCharacter *>(object),
                          false);
                        if (m_DamList[m_nDamagedObjNum].m_nDamage != -2)
                        {
                          m_DamList[m_nDamagedObjNum].m_nDamage = static_cast<int>(
                            static_cast<float>(m_DamList[m_nDamagedObjNum].m_nDamage)
                            * (static_cast<float>(nLimitRadius - distInt) / static_cast<float>(nLimitRadius)));
                          if (m_DamList[m_nDamagedObjNum].m_nDamage < 1)
                          {
                            continue;
                          }
                        }
                        ++m_nDamagedObjNum;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

void CAttack::SectorDamageProc(
  int nSkillLv,
  int nAttPower,
  int nAngle,
  int nShotNum,
  int nWeaponRange,
  int nEffAttPower,
  bool bUseEffBullet)
{
  const int maxRange = nWeaponRange;
  const int minRange = nWeaponRange / 4;

  if (!m_pp->pDst)
  {
    return;
  }

  m_DamList[0].m_pChar = m_pp->pDst;
  if (m_pp->bMatchless)
  {
    m_DamList[0].m_nDamage = m_pp->pDst->GetHP();
  }
  else
  {
    const int damageValue = bUseEffBullet
      ? m_pAttChar->GetAttackDamPoint(nEffAttPower, m_pp->nPart, m_pp->nTol, m_pp->pDst, m_pp->bBackAttack)
      : m_pAttChar->GetAttackDamPoint(nAttPower, m_pp->nPart, m_pp->nTol, m_pp->pDst, m_pp->bBackAttack);
    m_DamList[0].m_nDamage = damageValue;
  }
  m_nDamagedObjNum = 1;

  if (m_pAttChar->GetCurSecNum() == -1)
  {
    return;
  }

  bool inGuildBattle = false;
  if (nShotNum <= 1)
  {
    return;
  }

  CMapData *currentMap = m_pAttChar->m_pCurMap;
  _pnt_rect rect{};
  const int curSec = m_pAttChar->GetCurSecNum();
  currentMap->GetRectInRadius(&rect, 1, curSec);

  for (int secY = rect.nStarty; secY <= rect.nEndy; ++secY)
  {
    for (int secX = rect.nStartx; secX <= rect.nEndx; ++secX)
    {
      _sec_info *secInfo = currentMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * secY + secX;
      CObjectList *sectorList = currentMap->GetSectorListObj(m_pAttChar->m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      for (_object_list_point *node = sectorList->m_Head.m_pNext; node != &sectorList->m_Tail; node = node->m_pNext)
      {
        CGameObject *object = node->m_pItem;
        if (m_nDamagedObjNum >= 30 || m_nDamagedObjNum >= nShotNum)
        {
          return;
        }
        if (object->m_ObjID.m_byKind || object == m_pAttChar || object == m_pp->pDst || !object->m_bLive
            || object->m_bCorpse || !object->IsBeAttackedAble(false))
        {
          continue;
        }

        inGuildBattle = false;
        if (!CheckGuildBattleLimit(object, &inGuildBattle))
        {
          if (inGuildBattle)
          {
            goto CHECK_SECTOR_DAMAGE;
          }
          const int objRace = object->GetObjRace();
          const int attRace = m_pAttChar->GetObjRace();
          if (objRace != attRace || m_pAttChar->m_ObjID.m_byID)
          {
            goto CHECK_SECTOR_DAMAGE;
          }

          CPlayer *attPlayer = static_cast<CPlayer *>(m_pAttChar);
          if (object->m_ObjID.m_byID || static_cast<CPlayer *>(object)->IsPunished(1u, false)
              || (attPlayer)->IsChaosMode())
          {
            if (object->m_ObjID.m_byID != 11
                || ((static_cast<AutominePersonal *>(object)->GetOwner() != attPlayer)
                    && (attPlayer)->IsChaosMode()))
            {
              goto CHECK_SECTOR_DAMAGE;
            }
          }
        }
        continue;

CHECK_SECTOR_DAMAGE:
        if (object->IsBeDamagedAble(m_pAttChar))
        {
          CAnimus *recallAnimus = nullptr;
          if (!m_pAttChar->m_ObjID.m_byID)
          {
            recallAnimus = static_cast<CPlayer *>(m_pAttChar)->GetRecallAnimus();
          }
          if (!m_pAttChar->m_ObjID.m_byID || !recallAnimus || recallAnimus != object)
          {
            if (m_pAttChar->m_ObjID.m_byID
                || static_cast<CPlayer *>(m_pAttChar)->GetObjRace() != 1
                || (static_cast<CPlayer *>(m_pAttChar)->IsChaosMode())
                || (object == nullptr))
            {
              if (object->m_ObjID.m_byID != 7
                  && (m_pAttChar->IsAttackableInTown() || object->IsAttackableInTown()
                      || (!m_pAttChar->IsInTown() && !object->IsInTown())))
              {
                const float heightDiff = std::abs(m_pAttChar->m_fCurPos[1] - object->m_fCurPos[1]);
                if (heightDiff <= 350.0f)
                {
                  const float distance = GetSqrt(m_pAttChar->m_fCurPos, object->m_fCurPos);
                  const float width = object->GetWidth();
                  float adjustedDist = 0.0f;
                  if ((distance - width) > 0.0f)
                  {
                    const float distance2 = GetSqrt(m_pAttChar->m_fCurPos, object->m_fCurPos);
                    const float width2 = object->GetWidth();
                    adjustedDist = distance2 - width2;
                  }
                  const int distInt = static_cast<int>(adjustedDist);
                  if (distInt <= maxRange && distInt >= minRange)
                  {
                    if (IsCharInSector(
                          object->m_fCurPos,
                          m_pAttChar->m_fCurPos,
                          m_pp->pDst->m_fCurPos,
                          static_cast<float>(nAngle),
                          static_cast<float>(maxRange)))
                    {
                      m_DamList[m_nDamagedObjNum].m_pChar = static_cast<CCharacter *>(object);
                      if (m_pp->bMatchless)
                      {
                        m_DamList[m_nDamagedObjNum].m_nDamage = object->GetHP();
                        ++m_nDamagedObjNum;
                        continue;
                      }

                      const int attackPart = static_cast<CCharacter *>(object)->GetAttackRandomPart();
                      m_DamList[m_nDamagedObjNum].m_nDamage = m_pAttChar->GetAttackDamPoint(
                        nAttPower,
                        attackPart,
                        m_pp->nTol,
                        static_cast<CCharacter *>(object),
                        false);
                      if (m_DamList[m_nDamagedObjNum].m_nDamage != -2)
                      {
                        m_DamList[m_nDamagedObjNum].m_nDamage = static_cast<int>(
                          static_cast<float>(m_DamList[m_nDamagedObjNum].m_nDamage)
                          * (static_cast<float>(maxRange - distInt) / static_cast<float>(maxRange)));
                        if (m_DamList[m_nDamagedObjNum].m_nDamage < 1)
                        {
                          continue;
                        }
                      }
                      ++m_nDamagedObjNum;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

bool CAttack::CheckGuildBattleLimit(CGameObject *pObject, bool *pbInGuildBattle)
{
  if (pbInGuildBattle)
  {
    *pbInGuildBattle = false;
  }
  if (m_pAttChar->m_ObjID.m_byID)
  {
    return false;
  }
  if (pObject->m_ObjID.m_byKind)
  {
    return false;
  }

  CGameObject *targetPlayer = nullptr;
  if (!pObject->m_ObjID.m_byID)
  {
    targetPlayer = pObject;
  }
  if (!targetPlayer)
  {
    return false;
  }

  CPlayer *attacker = static_cast<CPlayer *>(m_pAttChar);
  CPlayer *targetPlayerChar = static_cast<CPlayer *>(targetPlayer);
  if (pbInGuildBattle && (targetPlayerChar->m_bInGuildBattle || attacker->m_bInGuildBattle))
  {
    *pbInGuildBattle = true;
  }
  if (!attacker->m_bInGuildBattle)
  {
    return targetPlayerChar->m_bInGuildBattle != false;
  }
  if (attacker->m_bTakeGravityStone)
  {
    return true;
  }
  if (!targetPlayerChar->m_bInGuildBattle)
  {
    return true;
  }
  if (targetPlayerChar->m_bTakeGravityStone)
  {
    return true;
  }
  return attacker->m_byGuildBattleColorInx == targetPlayerChar->m_byGuildBattleColorInx;
}

bool CAttack::IsCharInSector(float *chkpos, float *src, float *dest, float angle, float radius)
{
  if (*src == *dest && src[2] == dest[2])
  {
    return false;
  }
  if (*chkpos == *dest && chkpos[2] == dest[2])
  {
    return true;
  }

  float dirToCheck[3] = {
    chkpos[0] - dest[0],
    chkpos[1] - dest[1],
    chkpos[2] - dest[2],
  };

  const float distance =
    std::sqrt((dirToCheck[0] * dirToCheck[0]) + (dirToCheck[1] * dirToCheck[1]) + (dirToCheck[2] * dirToCheck[2]));
  if (distance > radius)
  {
    return false;
  }

  float dirToDest[3] = {
    dest[0] - src[0],
    dest[1] - src[1],
    dest[2] - src[2],
  };

  Normalize(dirToCheck);
  Normalize(dirToDest);

  const float angleDiff = std::acos((dirToCheck[0] * dirToDest[0]) + (dirToCheck[1] * dirToDest[1])
                                    + (dirToCheck[2] * dirToDest[2]))
    * 360.0f / 6.283184051513672f;
  return (angle / 2.0f) > angleDiff;
}

