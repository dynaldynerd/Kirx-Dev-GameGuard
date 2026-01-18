#pragma once

#include "IdaCompat.h"
#include "CMonsterSkill.h"

struct CMonster;

/* 1648 */
struct __cppobj CMonsterSkillPool
{
  CMonster *m_pMyMon;
  int m_nSize;
  CMonsterSkill m_MonSkill[16];
};
