#pragma once

#include "IdaCompat.h"

#include "CGameObject.h"
#include "CMyTimer.h"

struct _character_create_setdata;

struct __declspec(align(4)) _sf_continous
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

  static unsigned int GetSFContCurTime();
};

struct __declspec(align(8)) _effect_parameter
{
  struct __param_data;

  __param_data *m_pDataParam;
  bool m_bLock;

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
};

struct _effect_parameter::__param_data
{
  float m_fEff_Rate[62];
  float m_fEff_Plus[42];
  char m_bEff_State[29];
  float m_fEff_Have[83];
};

class CCharacter : public CGameObject
{
public:
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
  void Go();
  void Stop();
  void ResetSlot();
  void RemoveSFContEffect(unsigned __int8 byContCode, unsigned __int16 wListIndex, bool bInit, bool bAura);
  void AlterContDurSec(unsigned __int8 byContCode, unsigned __int16 wListIndex, unsigned int dwStartSec, unsigned __int16 wNewDur);
  void RemoveSFContHelpByEffect(int nContParamCode, int nContParamIndex);
  unsigned int CalcEffectBit(unsigned __int16 wEffectCode, unsigned __int16 wEffectIndex);
};
