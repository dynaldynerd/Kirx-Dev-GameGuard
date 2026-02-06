#pragma once

#include "IdaCompat.h"

class CCharacter;

/* 1638 */
class __cppobj CAggroNode
{
  friend class CMonsterAggroMgr;
  friend class CPlayer;
public:
  CAggroNode();
  void Init();
  void Set(CCharacter *pCharacter);
  void SetAggro(
    int nDam,
    float fAdd,
    int nAttackType,
    unsigned int dwAttackSerial,
    int bOtherPlayerSupport,
    int bFirstAttack,
    int bTempSkill);
  bool IsLive();

private:
  CCharacter *m_pCharacter;
  unsigned int m_dwObjectSerial;
  int m_nAggroData;
  int m_nDamageData;
  int m_nKingPowerDamage;
};

