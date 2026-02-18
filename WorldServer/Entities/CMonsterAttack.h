#pragma once

#include "IdaCompat.h"

#include "CAttack.h"

class CMonster;

/* 3483 */
class  CMonsterAttack : public CAttack
{
public:
  CMonster *m_pAttMonster;

  CMonsterAttack(CMonster *pThis);
  float ModifyMonsterAttFc(float fAttFc);
  void AttackMonsterGen(_attack_param *pParam, bool bMustMiss);
  void AttackMonsterSkill(_attack_param *pParam);
  void AttackMonsterForce(_attack_param *pParam);
  __int64 _CalcMonSkillAttPnt();
};
