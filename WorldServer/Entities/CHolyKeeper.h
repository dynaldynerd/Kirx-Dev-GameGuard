#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"

class CAttack;
class CPlayer;
struct _monster_fld;
struct _dummy_position;

class __cppobj CHolyKeeper : public CCharacter
{
public:
  int m_nHP;
  unsigned int m_dwLastDestroyTime;
  _monster_fld *m_pRec;
  _dummy_position *m_pPosCreate;
  _dummy_position *m_pPosActive;
  _dummy_position *m_pPosCenter;
  int m_nMasterRace;
  bool m_bExit;
  bool m_bChaos;
  unsigned int m_dwLastStopTime;
  int m_nDefPart[5];
  CPlayer *m_pLastMoveTarget;
  CAttack *m_at;
  _attack_param m_ap;
  bool m_bDamageAbleState;
  int m_nCurrLootIndex;
  int m_nEndLootIndex;
  int m_nCurrDropIndex;
  unsigned __int16 m_wMagnifications;
  unsigned __int16 m_wRange;
  unsigned __int16 m_wDropCntOnce;
  CMyTimer m_tmrDropTime;

  void Init(_object_id *pID);
};
