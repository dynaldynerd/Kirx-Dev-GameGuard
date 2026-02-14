#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

class CAttack;

/* 1779 */
class __cppobj CAnimus : public CCharacter
{
public:
  CAnimus();
  ~CAnimus();

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
  void SendMsg_RealMovePoint(int n) override;
  void SendMsg_Create();
  void SendMsg_Move();
  void SendMsg_Attack_Gen(CAttack *pAT);
  void SendMsg_LevelUp();
  void SendMsg_AnimusActHealInform(unsigned int dwDstSerial, __int16 nAddHP);
  __int64 SetDamage(int nDam, CCharacter *pDst, int nDstLv, bool bCrt);

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

  static bool SetStaticMember();
  void Init(_object_id *pID);
  bool Destroy();
  void SendMsg_Destroy();
  void MasterAttack_MasterInform(CCharacter *pDst);

  static CRecordData s_tblParameter[8];
  static unsigned int MAX_EXP[8];
  static int s_nLiveNum;
};

_animus_fld *GetAnimusFldFromLv(int nAnimusClass, unsigned int dwLv);

