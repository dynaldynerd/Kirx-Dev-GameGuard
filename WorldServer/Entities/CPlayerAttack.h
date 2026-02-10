#pragma once

#include "IdaCompat.h"
#include "CAttack.h"

class CPlayer;
struct _attack_param;

class __cppobj CPlayerAttack : public CAttack
{
public:
  CPlayer *m_pAttPlayer;

  CPlayerAttack(CPlayer *pThis);
  __int64 _CalcSkillAttPnt(bool bUseEffBullet);
  void AttackSkill(_attack_param *pParam, bool bUseEffBullet);
  void AttackUnit(_attack_param *pParam);
  void WPActiveAttackSkill(_attack_param *pParam);
  void WPActiveAttackForce(_attack_param *pParam);
};
