#pragma once

#include "IdaCompat.h"
#include "CMonsterSkill.h"

class CMonster;

/* 1648 */
class  CMonsterSkillPool
{
  CMonster *m_pMyMon;
  int m_nSize;
  CMonsterSkill m_MonSkill[16];

public:
  CMonsterSkillPool();
  ~CMonsterSkillPool();
  void Init();
  int Set(CMonster *pMyMonster);
  int InsertSkill(CMonsterSkill *skill);
  CMonsterSkill *GetMonSkill(unsigned int nIndex);
  CMonsterSkill *GetMonSkillKind(unsigned int nKind);
};

