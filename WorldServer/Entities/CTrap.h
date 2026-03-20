#pragma once

#include "IdaCompat.h"
#include "CCharacter.h"

class CPlayer;
class CAttack;
class CMapData;
struct _trap_create_setdata;

/* 1746 */
class  CTrap : public CCharacter
{
public:
  CTrap();
  ~CTrap();

  int GetAttackDP() override;
  float GetAttackRange() override;
  int GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart) override;
  float GetDefFacing(int nPart) override;
  float GetDefGap(int nPart) override;
  int GetDefSkill(bool bBackAttackDamage) override;
  int GetFireTol() override;
  int GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack) override;
  int GetHP() override;
  int GetLevel() override;
  int GetMaxHP() override;
  char *GetObjName() override;
  int GetObjRace() override;
  int AttackableHeight();
  int GetSoilTol() override;
  int GetWaterTol() override;
  float GetWeaponAdjust() override;
  int GetWeaponClass() override;
  float GetWidth() override;
  int GetWindTol() override;
  bool IsBeAttackedAble(bool bFirst) override;
  bool IsInTown() override;
  void Loop() override;
  void OutOfSec() override;
  void RecvKillMessage(CCharacter *pDier) override;
  void SendMsg_FixPosition(int n) override;
  using CCharacter::Destroy;
  bool Destroy(unsigned __int8 byDesType);
  void Attack(CCharacter *pTarget);
  CCharacter *SearchNearEnemy();
  void CheckTranspar();
  void SendMsg_Destroy(unsigned __int8 byDesType);
  void SendMsg_Create();
  void SendMsg_Attack(CAttack *pAt);
  void SendMsg_TrapCompleteInform();
  void SendMsg_AlterTranspar(bool bTranspar);
  bool Create(_trap_create_setdata *pData);
  static bool IsHaveEmpty();
  unsigned int GetNewSerial();

  static int s_nLiveNum;
  static unsigned int s_dwSerialCnt;

  int m_nHP;
  CPlayer *m_pMaster;
  unsigned __int8 m_byRaceCode;
  unsigned int m_dwMasterSerial;
  char m_wszMasterName[17];
  char m_aszMasterName[17];
  long double m_dMasterPvPPoint;
  unsigned int m_dwStartMakeTime;
  bool m_bComplete;
  bool m_bBreakTransparBuffer;
  unsigned int m_dwLastDestroyTime;
  int m_nTrapMaxAttackPnt;

  bool Init(_object_id *pID);
  void MasterNetClose(long double dPvPPoint);
  void MasterReStart(CPlayer *pMaster);
  int SetDamage(
    int nDam,
    CCharacter *pDst,
    int nDstLv,
    bool bCrt,
    int nAttackType,
    unsigned int dwAttackSerial,
    bool bJadeReturn) override;
  int SetDamage(int nDam, CCharacter *pDst, int nDstLv);

private:
  void SendMsg_FixPositionImpl(int n);
};

CTrap *CreateTrap(
  CMapData *pMap,
  unsigned __int16 wLayer,
  float *fPos,
  CPlayer *pMaster,
  int nTrapItemIndex);

