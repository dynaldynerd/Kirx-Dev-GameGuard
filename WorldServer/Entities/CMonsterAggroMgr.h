#pragma once

#include "IdaCompat.h"
#include "CAggroNode.h"

class CCharacter;
class CMonster;

/* 1639 */
class __cppobj CMonsterAggroMgr
{
  CCharacter *m_pTopAggroCharacter;
  CCharacter *m_pTopDamageCharacter;
  CCharacter *m_pKingPowerDamageCharacter;
  CAggroNode m_AggroPool[10];
  unsigned int m_dwAggroCount;
  unsigned int m_dwAllResetLastTime;
  unsigned int m_dwShortRankLastTime;
  unsigned int m_dwAllResetTimer;
  unsigned int m_dwShortRankTimer;
  CMonster *m_pMonster;
};

