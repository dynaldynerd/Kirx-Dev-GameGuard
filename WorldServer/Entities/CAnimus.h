#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

/* 1779 */
class __cppobj CAnimus : CCharacter
{
public:
  unsigned __int8 m_byClassCode;
  int m_nHP;
  int m_nFP;
  unsigned __int64 m_dwExp;
  CPlayer *m_pMaster;
  unsigned int m_dwMasterSerial;
  char m_wszMasterName[17];
  char m_aszMasterName[17];
  unsigned __int8 m_byRoleCode;
  unsigned int m_dwLastDestroyTime;
  float m_fMoveSpeed;
  unsigned __int8 m_byPosRaceTown;
  CMapData *m_pBeforeTownCheckMap;
  float m_fBeforeTownCheckPos[2];
  unsigned int m_dwStunTime;
  unsigned int m_dwBeAttackedTargetTime;
  CCharacter *m_pNextTarget;
  int m_nMaxAttackPnt;
  unsigned int m_tmNextEatMasterFP;
  _animus_fld *m_pRecord;
  int m_nMaxHP;
  int m_nMaxFP;
  float m_Mightiness;
  int m_DefPart[5];
  unsigned int m_dwAIMode;
  CCharacter *m_pTarget;
  CAITimer m_AITimer[3];
  SKILL m_Skill[2];

  static void SetStaticMember();
  void Init(_object_id *pID);
};

