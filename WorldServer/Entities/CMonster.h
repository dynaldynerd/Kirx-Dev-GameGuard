#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"
#include "CLootingMgr.h"
#include "CMonsterAggroMgr.h"
#include "CMonsterHierarchy.h"
#include "CMonsterSkillPool.h"
#include "CMonsterAI.h"
#include "CLuaSignalReActor.h"

/* 1635 */
class __cppobj CMonster : CCharacter
{
public:
  bool m_bOper;
  bool m_bApparition;
  bool m_bDungeon;
  unsigned int m_dwLastDestroyTime;
  unsigned int m_dwDestroyNextTime;
  unsigned int m_dwLastRecoverTime;
  float m_fCreatePos[3];
  float m_fLookAtPos[3];
  bool m_bRobExp;
  bool m_bRewardExp;
  bool m_bStdItemLoot;
  _mon_active *m_pActiveRec;
  _monster_fld *m_pMonRec;
  _dummy_position *m_pDumPosition;
  int m_nHP;
  CLootingMgr m_LootMgr;
  CMonsterAggroMgr m_AggroMgr;
  CMonsterHierarchy m_MonHierarcy;
  MonsterSFContDamageToleracne m_SFContDamageTolerance;
  unsigned __int8 m_byCreateDate[4];
  unsigned int m_LifeMax;
  unsigned int m_LifeCicle;
  $B1473DED9FF3433080ADDC1400B1BAA4 ___u23;
  EmotionPresentationChecker m_EmotionPresentationCheck;
  float m_fYAngle;
  float m_fStartLookAtPos[3];
  bool m_bRotateMonster;
  MonsterStateData m_MonsterStateData;
  MonsterStateData m_BeforeMonsterStateData;
  CCharacter *m_pTargetChar;
  CMonsterSkillPool m_MonsterSkillPool;
  int m_DefPart[5];
  int m_nEventItemNum;
  _event_loot_item m_eventItem[16];
  _event_respawn *m_pEventRespawn;
  _event_set *m_pEventSet;
  CMonsterAI m_AI;
  CLuaSignalReActor m_LuaSignalReActor;

  void Init(_object_id *pID);
};

