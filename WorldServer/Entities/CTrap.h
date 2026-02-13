#pragma once

#include "IdaCompat.h"
#include "CCharacter.h"

class CPlayer;

/* 1746 */
class __cppobj CTrap : public CCharacter
{
public:
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
  void Loop() override;
  void OutOfSec() override;
  void SendMsg_FixPosition(int n) override;

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

  void Init(_object_id *pID);
  void MasterNetClose(long double dPvPPoint);
  void MasterReStart(CPlayer *pMaster);
  __int64 SetDamage(
    int nDam,
    CCharacter *pDst,
    int nDstLv,
    bool bCrt,
    int nAttackType,
    unsigned int dwAttackSerial,
    bool bJadeReturn) override;
  __int64 SetDamage(int nDam, CCharacter *pDst, int nDstLv);
};

