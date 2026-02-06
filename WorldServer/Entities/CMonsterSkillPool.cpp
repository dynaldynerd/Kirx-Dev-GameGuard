#include "pch.h"

#include "CMonsterSkillPool.h"

#include "CMainThread.h"
#include "CMonster.h"
#include "CMonsterSPGroupTable.h"
#include "force_fld.h"
#include "monster_fld.h"
#include "monster_sp_fld.h"
#include "skill_fld.h"

CMonsterSkillPool::CMonsterSkillPool()
{
  Init();
}

CMonsterSkillPool::~CMonsterSkillPool() = default;

void CMonsterSkillPool::Init()
{
  m_pMyMon = nullptr;
  m_nSize = 0;
  for (int j = 0; j < 16; ++j)
  {
    m_MonSkill[j].Init();
  }
}

__int64 CMonsterSkillPool::Set(CMonster *pMyMonster)
{
  CMonsterSkillPool::Init();
  if (!pMyMonster)
  {
    return 0;
  }

  m_pMyMon = pMyMonster;
  CMonsterSkill skill;
  _monster_fld *monsterFld = m_pMyMon->m_pMonRec;
  if (!monsterFld)
  {
    return 0;
  }

  _monster_sp_group *spGroup = CMonsterSPGroupTable::GetRecord(&g_Main.m_MonsterSPGroupTable, monsterFld->m_dwIndex);
  if (!spGroup)
  {
    return 0;
  }

  if (spGroup->m_wCount > 0xF)
  {
    return 0;
  }

  for (unsigned __int16 index = 0; index < spGroup->m_wCount; ++index)
  {
    _monster_sp_fld *spCont = spGroup->m_pSPData[index];
    auto spField = &spGroup->m_SPDataFld[index];
    if (!spCont)
    {
      return 0;
    }
    if (!spField)
    {
      return 0;
    }

    float attackExt = monsterFld->m_fAttExt;
    if (spCont->m_fSpecialRange != -1.0f)
    {
      attackExt = spCont->m_fSpecialRange;
    }

    if (spCont->m_nSpecialAttType)
    {
      if (spCont->m_nSpecialAttType == 1)
      {
        skill.Init();
        const float actDelay = (spField->m_ForceFld->m_fActDelay <= static_cast<float>(1000 * spCont->m_nMotiveTime))
                                 ? static_cast<float>(1000 * spCont->m_nMotiveTime)
                                 : spField->m_ForceFld->m_fActDelay;
        const unsigned int motionTime = static_cast<unsigned int>(monsterFld->m_fAttMoTime1);
        const int motiveValue = spCont->m_nDstType;
        const int motive = spCont->m_nMotiveValue;
        const int motiveCondition = spCont->m_nMotiveCondition;
        CMonsterSkill::SetForce(
          &skill,
          monsterFld,
          spCont,
          spCont->m_nAttLv,
          spField->m_ForceFld,
          static_cast<unsigned int>(actDelay),
          attackExt,
          motionTime,
          motiveCondition,
          motive,
          motiveValue);
        CMonsterSkillPool::InsertSkill(this, &skill);
      }
      else if (spCont->m_nSpecialAttType == 2)
      {
        skill.Init();
        const float actDelay = (spField->m_ForceFld->m_fActDelay <= static_cast<float>(1000 * spCont->m_nMotiveTime))
                                 ? static_cast<float>(1000 * spCont->m_nMotiveTime)
                                 : spField->m_ForceFld->m_fActDelay;
        const unsigned int motionTime = static_cast<unsigned int>(monsterFld->m_fAttMoTime1);
        const int skillDestType = spCont->m_nDstType;
        const int motiveValue = spCont->m_nMotiveValue;
        const int motive = spCont->m_nMotiveCondition;
        CMonsterSkill::SetSkill(
          &skill,
          monsterFld,
          spCont,
          spCont->m_nAttLv,
          2,
          spField->m_SkillFld,
          static_cast<unsigned int>(actDelay),
          attackExt,
          motionTime,
          motive,
          motiveValue,
          skillDestType);
        CMonsterSkillPool::InsertSkill(this, &skill);
      }
    }
    else
    {
      skill.Init();
      const float actDelay = (spField->m_ForceFld->m_fActDelay <= static_cast<float>(1000 * spCont->m_nMotiveTime))
                               ? static_cast<float>(1000 * spCont->m_nMotiveTime)
                               : spField->m_ForceFld->m_fActDelay;
      const unsigned int motionTime = static_cast<unsigned int>(monsterFld->m_fAttMoTime1);
      const int skillDestType = spCont->m_nDstType;
      const int motiveValue = spCont->m_nMotiveValue;
      const int motive = spCont->m_nMotiveCondition;
      CMonsterSkill::SetSkill(
        &skill,
        monsterFld,
        spCont,
        spCont->m_nAttLv,
        0,
        spField->m_SkillFld,
        static_cast<unsigned int>(actDelay),
        attackExt,
        motionTime,
        motive,
        motiveValue,
        skillDestType);
      CMonsterSkillPool::InsertSkill(this, &skill);
    }
  }

  skill.Init();
  CMonsterSkill::SetGen(
    &skill,
    monsterFld,
    1,
    static_cast<unsigned int>(monsterFld->m_fAttSpd),
    monsterFld->m_fAttExt,
    static_cast<unsigned int>(monsterFld->m_fAttMoTime1));
  CMonsterSkillPool::InsertSkill(this, &skill);
  return 1;
}

__int64 CMonsterSkillPool::InsertSkill(CMonsterSkill *skill)
{
  if (m_nSize >= 16)
  {
    return 0;
  }
  if (!skill->m_bExit)
  {
    return 0;
  }
  for (int j = 0; j < 16; ++j)
  {
    if (!m_MonSkill[j].m_bExit)
    {
      m_MonSkill[j].Copy(skill);
      ++m_nSize;
      return 1;
    }
  }
  return 0;
}

CMonsterSkill *CMonsterSkillPool::GetMonSkill(unsigned int nIndex)
{
  if (nIndex < 0x10 && m_MonSkill[nIndex].m_bExit)
  {
    return &m_MonSkill[nIndex];
  }
  return nullptr;
}

CMonsterSkill *CMonsterSkillPool::GetMonSkillKind(unsigned int nKind)
{
  if (nKind < 4)
  {
    for (int j = 0; j < 16; ++j)
    {
      if (m_MonSkill[j].m_bExit && m_MonSkill[j].m_nSFCode == static_cast<int>(nKind))
      {
        return &m_MonSkill[j];
      }
    }
  }
  return nullptr;
}

