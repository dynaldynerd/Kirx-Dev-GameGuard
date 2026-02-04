#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "ObjectCreateSetData.h"

class CAttack;
class CPlayer;
struct _monster_fld;
struct _dummy_position;

class __cppobj CHolyKeeper : public CCharacter
{
public:
  CHolyKeeper();

  static int s_nLiveNum;
  static unsigned int s_dwSerialCnt;

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
  bool Create(_keeper_create_setdata *pData, int nCreateType);
  bool Destroy(unsigned __int8 byDestroyCode, CCharacter *pAtter);
  bool Exit();
  void SetMaxHP(int nMaxHP);
  void SetHP(int nHP);
  void SetDamageAbleState(bool bDamageAbState);
  void SetStateChaos();
  void SendMsg_Create();
  void SendMsg_Destroy(unsigned __int8 byDesType);
  void SendMsg_Move();
  void SetDropItem();
  void DropItem();
};
