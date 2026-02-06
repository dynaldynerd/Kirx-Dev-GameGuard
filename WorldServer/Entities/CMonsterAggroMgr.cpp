#include "pch.h"

#include "CMonsterAggroMgr.h"

#include "CMonster.h"
#include "WorldServerUtil.h"

void CMonsterAggroMgr::Init()
{
  for (int j = 0; j < 10; ++j)
  {
    CAggroNode::Init(&m_AggroPool[j]);
  }
  m_dwAggroCount = 0;
  m_dwAllResetLastTime = 0;
  m_dwShortRankLastTime = 0;
  m_dwShortRankTimer = CMonster::GetAggroShortTime(m_pMonster);
  m_dwAllResetTimer = CMonster::GetAggroResetTime(m_pMonster);
  m_pTopDamageCharacter = nullptr;
  m_pTopAggroCharacter = nullptr;
  m_pKingPowerDamageCharacter = nullptr;
}

void CMonsterAggroMgr::ResetAggro()
{
  m_dwShortRankLastTime = m_dwShortRankTimer + GetLoopTime();
}

CCharacter *CMonsterAggroMgr::GetTopAggroCharacter()
{
  if (m_pTopAggroCharacter && m_pTopAggroCharacter->m_bLive && !m_pTopAggroCharacter->m_bCorpse)
  {
    return m_pTopAggroCharacter;
  }
  m_pTopAggroCharacter = nullptr;
  return nullptr;
}

