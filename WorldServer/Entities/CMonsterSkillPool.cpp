#include "pch.h"

#include "CMonsterSkillPool.h"

void CMonsterSkillPool::Init()
{
  m_pMyMon = nullptr;
  m_nSize = 0;
  for (int j = 0; j < 16; ++j)
  {
    m_MonSkill[j].Init();
  }
}

