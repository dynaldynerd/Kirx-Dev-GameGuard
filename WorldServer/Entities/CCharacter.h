#pragma once

#include "IdaCompat.h"

#include "CGameObject.h"
#include "CMyTimer.h"
#include "EffectParameterIndex.h"

struct _character_create_setdata;
struct _skill_fld;
struct _force_fld;

class CCharacter;
class CPlayer;

struct _sf_continous
{
  bool m_bExist;
  unsigned __int8 m_byEffectCode;
  unsigned __int16 m_wEffectIndex;
  unsigned __int8 m_byLv;
  unsigned int m_dwStartSec;
  unsigned __int16 m_wDurSec;
  unsigned int m_dwEffSerial;
  int m_nCumulCounter;
  unsigned int m_dwPlayerSerial;
  char m_wszPlayerName[17];

  _sf_continous();
  static unsigned int GetSFContCurTime();
};

struct _effect_parameter
{
  struct __param_data;

  __param_data *m_pDataParam = nullptr;
  bool m_bLock = false;
  static int m_nInitCount;

  _effect_parameter();
  ~_effect_parameter();
  void AllocEffParam();
  void InitEffParam();
  void InitEffHave();
  float GetEff_Rate(unsigned int nParamIndex);
  float GetEff_Plus(unsigned int nParamIndex);
  float GetEff_Have(unsigned int nParamIndex);
  bool GetEff_State(unsigned int nParamIndex);
  bool SetEff_Rate(unsigned int nParamIndex, float fVar, bool bAdd);
  bool SetEff_Plus(unsigned int nParamIndex, float fVar, bool bAdd);
  bool SetEff_State(unsigned int nParamIndex, bool bVar);
  void SetLock(bool bLock);
};

struct _effect_parameter::__param_data
{
  float m_fEff_Rate[66];
  float m_fEff_Plus[42];
  char m_bEff_State[29];
  float m_fEff_Have[92];
};

struct _tmp_effected_list
{
  CCharacter *pMem;
  unsigned __int8 byRetCode;
  unsigned __int16 wEffectValue;
};

class CCharacter : public CGameObject
{
public:
  CCharacter();
  ~CCharacter();

  float m_fTarPos[3];
  int m_AroundNum;
  CCharacter *m_AroundSlot[5];
  unsigned int m_dwNextGenAttackTime;
  _sf_continous m_SFCont[2][8];
  _sf_continous m_SFContAura[2][8];
  unsigned int m_dwEffSerialCounter;
  bool m_bLastContEffectUpdate;
  unsigned __int16 m_wLastContEffect;
  _effect_parameter m_EP;
  unsigned __int16 m_wEffectTempValue;
  unsigned int m_dwPlayerSerial;
  char m_wszPlayerName[17];
  int m_nContEffectSec;
  CMyTimer m_tmrSFCont;

  bool Create(_character_create_setdata *pData);
  bool Destroy();
  void Init(_object_id *pID);
  bool SetTarPos(float *fTarPos, bool bColl);
  float CalcDistForSec(float fSec, float fSpeed);
  void Move(float fSpeed);
  void MoveBreak(float fSpeed);
  void Go();
  void Stop();
  unsigned int GetNextGenAttTime();
  void SetNextGenAttTime(unsigned int dwNextTime);
  void ResetSlot();
  int GetSlot(CCharacter *p);
  int RemoveSlot(CCharacter *p);
  int GetNearEmptySlot(unsigned int pos, float dist, float *cur, float *target);
  int InsertSlot(CCharacter *p, int pos);
  void BreakStealth();
  int GetTotalTol(unsigned __int8 byAttTolType, int nDamPoint);
  int GetAttackDamPoint(int nAttPnt, int nAttPart, int nTolType, CCharacter *pDst, bool bBackAttack);
  int GetAttackRandomPart();
  void SendMsg_AttackActEffect(unsigned __int8 byActEffect, CCharacter *pDamer);
  void SendMsg_LastEffectChangeInform();
  void SendMsg_RobedHP(const CCharacter *pkPerform, unsigned __int16 wRobedHP);
  bool GetStealth(bool bInvisible);
  bool GetInvisible();
  void SFContInit();
  bool RemoveAllContinousEffect();
  bool RemoveAllContinousEffectGroup(unsigned int uiEffectCodeType);
  void RemoveSFContEffect(unsigned __int8 byContCode, unsigned __int16 wListIndex, bool bInit, bool bAura);
  void UpdateSFCont();
  void AlterContDurSec(unsigned __int8 byContCode, unsigned __int16 wListIndex, unsigned int dwStartSec, unsigned __int16 wNewDur);
  void RemoveSFContHelpByEffect(int nContParamCode, int nContParamIndex);
  unsigned __int16 CalcEffectBit(unsigned __int16 wEffectCode, unsigned __int16 wEffectIndex);
  int FindEffectDst(
    int nEffectCode,
    int nAreaType,
    int nLv,
    bool bBenefit,
    CCharacter *pOriDst,
    char *psActableDst,
    CCharacter **ppDsts);
  int FindPotionEffectDst(
    int nAreaType,
    int nEffectAreaVal,
    bool bBenefit,
    CCharacter *pOriDst,
    char *psActableDst,
    CCharacter **ppDsts,
    bool *pbPath);
  int _GetAreaEffectMember(
    CCharacter *pOriDst,
    bool bBenefit,
    int nLimitRadius,
    float *pTar,
    char *psActableDst,
    CCharacter **ppDsts);
  int _GetPartyEffectMember(CCharacter *pOriDst, bool bCircle, CCharacter **ppDsts);
  int _GetFlashEffectMember(
    CCharacter *pOriDst,
    bool bBenefit,
    int nLimitRadius,
    int nLimitAngle,
    CCharacter *pOriTar,
    char *psActableDst,
    CCharacter **ppDsts);
  bool IsEffectableDst(char *psActableDst, CCharacter *pDst);
  bool IsDamageEffect(unsigned int uiEffectCodeType, unsigned __int16 wEffectIndex);
  bool IsPotionEffectableDst(char *psActableDst, CPlayer *pDst);
  bool AssistSkill(
    CCharacter *pDstChar,
    int nEffectCode,
    _skill_fld *pSkillFld,
    int nSkillLv,
    unsigned __int8 *pbyErrorCode,
    bool *pbUpMty);
  bool AssistForce(
    CCharacter *pDstChar,
    _force_fld *pForceFld,
    int nForceLv,
    unsigned __int8 *pbyErrorCode,
    bool *pbUpMty);
  bool AssistSkillToOne(CCharacter *pDst, int nEffectCode, _skill_fld *pSkillFld, int nSkillLv);
  bool AssistForceToOne(CCharacter *pDst, _force_fld *pForceFld, int nForceLv);
  virtual unsigned __int8 InsertSFContEffect(
    unsigned __int8 byContCode,
    unsigned __int8 byEffectCode,
    unsigned int dwEffectIndex,
    unsigned __int16 wDurSec,
    unsigned __int8 byLv,
    bool *pbUpMty,
    CPlayer *pActChar);
  void SendMsg_StunInform() override;
  void _set_sf_cont(
    _sf_continous *pCont,
    unsigned __int8 byEffectCode,
    unsigned __int16 wEffectIndex,
    unsigned __int8 byLv,
    unsigned int dwStartSec,
    unsigned __int16 wDurSec,
    int nCumulCount);
};

int _CheckCumulativeSF(
  unsigned __int8 byEffectCode,
  int dwEffectIndex,
  int *nCumulMax,
  unsigned int *nEffectCount,
  char **pstrLinkCode);

