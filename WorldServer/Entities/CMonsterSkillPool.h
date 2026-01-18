#pragma once

#include "IdaCompat.h"
#include "CMonsterSkill.h"

class CMonster;

/* 1648 */
class __cppobj CMonsterSkillPool
{
  CMonster *m_pMyMon;
  int m_nSize;
  CMonsterSkill m_MonSkill[16];
};

