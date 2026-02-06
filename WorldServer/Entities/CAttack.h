#pragma once

#include "IdaCompat.h"

class CCharacter;
class CGameObject;
class CMonster;
struct _attack_param;
class CRecordData;

#ifndef BE_DAMAGED_CHAR_DEFINED
#define BE_DAMAGED_CHAR_DEFINED
/* 2358 */
struct __cppobj __declspec(align(8)) _be_damaged_char
{
  CCharacter *m_pChar;
  int m_nDamage;
  bool m_bActiveSucc;
  int m_nActiveDamage;
};
#endif

/* 2359 */
class __cppobj __declspec(align(8)) CAttack
{
  friend class CMonster;
protected:
  _attack_param *m_pp;
  CCharacter *m_pAttChar;
  bool m_bIsCrtAtt;
  bool m_bActiveSucc;
  int m_nDamagedObjNum;
  _be_damaged_char m_DamList[30];
  bool m_bFailure;

public:
  CAttack(CCharacter *pThis);
  static CRecordData *s_pSkillData;
  static _attack_param s_DefParam;
  static void SetStaticMember(CRecordData *effectTable);

  __int64 _CalcGenAttPnt(bool bUseEffBullet);
  __int64 _CalcForceAttPnt(bool bUseEffBullet);
  __int64 MonsterCritical_Exception_Rate(CMonster *pCharacter, bool bBackAttack);
  void CalcAvgDamage();
  void FlashDamageProc(
    int nLimDist,
    int nAttPower,
    int nAngle,
    int nEffAttPower,
    bool bUseEffBullet);
  void AreaDamageProc(
    int nLimitRadius,
    int nAttPower,
    float *pTar,
    int nEffAttPower,
    bool bUseEffBullet);
  void SectorDamageProc(
    int nSkillLv,
    int nAttPower,
    int nAngle,
    int nShotNum,
    int nWeaponRange,
    int nEffAttPower,
    bool bUseEffBullet);
  bool CheckGuildBattleLimit(CGameObject *pObject, bool *pbInGuildBattle);
  static bool IsCharInSector(float *chkpos, float *src, float *dest, float angle, float radius);
};

extern float fR;
extern float fRLf;
extern float fRMf;
extern float fR_0;
extern float fRLf_0;
extern float fRMf_0;
extern float fR_1;
extern float fRLf_1;
extern float fRMf_1;

extern int s_nLimitDist[8];
extern int s_nLimitRadius[8];
extern int s_nLimitAngle[2][8];
extern int s_nLimitRadius_0[16];
extern int s_nLimitAngle_0;
extern int s_nLimitAngle_1;

extern int s_Mon_nLimitDist[8];
extern int s_Mon_nLimitRadius[8];
extern int s_Mon_nLimitAngle[2][8];
