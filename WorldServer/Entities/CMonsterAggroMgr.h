#pragma once

#include "IdaCompat.h"
#include "CAggroNode.h"

class CCharacter;
class CMonster;

/* 1639 */
class __cppobj CMonsterAggroMgr
{
  friend class CPlayer;
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

public:
  void Init();
  void ResetAggro();
  CCharacter *GetTopAggroCharacter();
  void OnlyOnceInit(CMonster *pMonster);
  void SetAggro(
    CCharacter *pCharacter,
    int nDam,
    int nAttackType,
    unsigned int dwAttackSerial,
    int bOtherPlayerSupport,
    int bTempSkill);
  CAggroNode *SearchAggroNode(CCharacter *pCharacter);
  void SendChangeAggroData();

private:
  CAggroNode *_SearchAggroNode(CCharacter *pCharacter);
  CAggroNode *_GetBlinkNode();
};

