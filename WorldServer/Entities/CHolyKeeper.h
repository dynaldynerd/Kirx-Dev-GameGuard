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
  ~CHolyKeeper();

  __int64 GetAttackDP() override;
  float GetAttackRange() override;
  __int64 GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart) override;
  float GetDefFacing(int nPart) override;
  float GetDefGap(int nPart) override;
  __int64 GetDefSkill(bool bBackAttackDamage) override;
  __int64 GetFireTol() override;
  __int64 GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack) override;
  __int64 GetHP() override;
  __int64 GetLevel() override;
  __int64 GetMaxHP() override;
  char *GetObjName() override;
  __int64 GetObjRace() override;
  __int64 GetSoilTol() override;
  __int64 GetWaterTol() override;
  float GetWeaponAdjust() override;
  __int64 GetWeaponClass() override;
  float GetWidth() override;
  __int64 GetWindTol() override;
  bool IsBeAttackedAble(bool bFirst) override;
  char IsBeDamagedAble(CCharacter *pAtter) override;
  void Loop() override;
  void OutOfSec() override;
  void SendMsg_FixPosition(int n) override;
  void SendMsg_RealMovePoint(int n) override;

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
  void SendMsg_Attack();
  void SetDropItem();
  void DropItem();
  __int64 SetDamage(
    int nDam,
    CCharacter *pDst,
    int nDstLv,
    bool bCrt,
    int nAttackType,
    unsigned int dwAttackSerial,
    bool bJadeReturn) override;
  __int64 SetDamage(int nDam, CCharacter *pDst, int nDstLv);
  void CheckAttack();
  void CheckExit();
  void CheckMove();
  void CheckCurPos();
  CPlayer *SearchMoveTarget();
  CCharacter *SearchAttackTarget();
  float *GetAttackPivot();
};
