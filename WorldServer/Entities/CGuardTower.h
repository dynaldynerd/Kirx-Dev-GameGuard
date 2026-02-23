#pragma once

#include "IdaCompat.h"
#include "ObjectCreateSetData.h"
#include "CPlayer.h"
#include "TEMP_WAIT_TOWER.h"

class CAttack;

struct  _tower_create_setdata : _character_create_setdata
{
  int nHP;
  CPlayer *pMaster;
  unsigned __int8 byRaceCode;
  _STORAGE_LIST::_db_con *pItem;
  int nIniIndex;
  bool bQuick;

  _tower_create_setdata()
  {
    nHP = 0;
    pMaster = nullptr;
    byRaceCode = 0;
    pItem = nullptr;
    nIniIndex = 0;
    bQuick = false;
  }
};

/* 1742 */
class  CGuardTower : public CCharacter
{
public:
  __int64 AttackableHeight();
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
  __int64 SetDamage(
    int nDam,
    CCharacter *pDst,
    int nDstLv,
    bool bCrt,
    int nAttackType,
    unsigned int dwAttackSerial,
    bool bJadeReturn) override;

  static int s_nLiveNum;
  static unsigned int s_dwSerialCnt;
  static unsigned int s_dwOldTick_CheckTemp;
  static __TEMP_WAIT_TOWER s_Temp[MAX_PLAYER];

  int m_nHP;
  CPlayer *m_pMasterTwr;
  unsigned int m_dwMasterSerial;
  unsigned __int8 m_byRaceCode;
  _STORAGE_LIST::_db_con *m_pItem;
  unsigned __int16 m_wItemSerial;
  bool m_bSystemStruct;
  int m_nIniIndex;
  unsigned int m_dwStartMakeTime;
  bool m_bComplete;
  bool m_bQuick;
  CCharacter *m_pTarget;
  CCharacter *m_pMasterSetTarget;
  unsigned int m_dwLastDestroyTime;

  bool Create(_tower_create_setdata *pData);
  void Init(_object_id *pID);
  bool Destroy(unsigned __int8 byDesType, bool bSystemBack);
  void SendMsg_Destroy(unsigned __int8 byDesType);
  void SendMsg_Create();
  void SendMsg_Attack(CAttack *pAt);
  void SendMsg_TowerCompleteInform();
  void NotifyOwnerAttackInform(CCharacter *pDst);
  bool IsValidTarget();
  CCharacter *SearchNearEnemy();
  void Attack(CCharacter *pTarget);
  static void OnLoop_Static();
  static __int64 GetNewSerial();
};

CGuardTower *CreateSystemTower(
  CMapData *pMap,
  unsigned __int16 wLayer,
  float *fPos,
  int nTowerIndex,
  unsigned __int8 byRaceCode,
  int nIniIndex);


