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

public:
  CMonsterSkillPool();
  ~CMonsterSkillPool();
  void Init();
  __int64 Set(CMonster *pMyMonster);
  __int64 InsertSkill(CMonsterSkill *skill);
  CMonsterSkill *GetMonSkill(unsigned int nIndex);
  CMonsterSkill *GetMonSkillKind(unsigned int nKind);
};

