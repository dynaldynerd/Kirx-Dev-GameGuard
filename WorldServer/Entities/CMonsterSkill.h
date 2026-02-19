#pragma once

#include "IdaCompat.h"

struct _base_fld;
struct _monster_fld;
struct _monster_sp_fld;
struct _skill_fld;
struct _force_fld;

/* 1647 */
class  CMonsterSkill
{
public:
  bool m_bExit;
  int m_UseType;
  int m_nSFCode;
  unsigned __int16 m_wSFIndex;
  _base_fld *m_pSF_Fld;
  _monster_sp_fld *m_pSPConst;
  unsigned int m_BefTime;
  unsigned int m_dwDelayTime;
  unsigned int m_dwCastDelay;
  float m_fAttackDist;
  int m_nMotive;
  int m_nMotivevalue;
  int m_nCaseType;
  int m_nAccumulationCount;
  int m_nSFLv;
  int m_Element;
  int m_StdDmg;
  int m_MinDmg;
  int m_MaxDmg;
  int m_MinProb;
  int m_MaxProb;
  CMonsterSkill();
  ~CMonsterSkill();
  void Init();
  void Copy(const CMonsterSkill *other);
  __int64 SetSkill(
    _monster_fld *pMonsterFld,
    _monster_sp_fld *pSPCont,
    int nSFLv,
    int nEffectType,
    _skill_fld *pSkillFld,
    unsigned int dwDelayTime,
    float fAttackDist,
    unsigned int dwCastDelay,
    int nMotive,
    int nMotiveValue,
    int skillDestType);
  __int64 SetForce(
    _monster_fld *pMonsterFld,
    _monster_sp_fld *pSPCont,
    int nSFLv,
    _force_fld *pForceFld,
    unsigned int dwDelayTime,
    float fAttackDist,
    unsigned int dwCastDelay,
    int nMotive,
    int nMotiveValue,
    int skillDestType);
  __int64 SetGen(
    _monster_fld *pMonsterFld,
    int nSFLv,
    unsigned int dwDelayTime,
    float fAttackDist,
    unsigned int dwCastDelay);
  __int64 Use(unsigned int dwUsedTime, bool bCount);
  bool IsExit();
  bool IsAttackAble();
  __int64 GetType();
  __int64 GetUseType();
  __int64 GetMotive();
  __int64 GetMotiveValue();
  __int64 GetExceptMotive();
  __int64 GetExceptMotiveValue();
  __int64 GetDstCaseType();
  __int64 GetBeforeTime();
  __int64 GetNextActionDelayTime();
  __int64 GetSPActionProbability();
  __int64 GetSPLimitCount();
  __int64 GetSFLv();
  __int64 GetElement();
  __int64 GetStdDmg();
  __int64 GetMinDmg();
  __int64 GetMaxDmg();
  __int64 GetMinProb();
  __int64 GetMaxProb();
  float GetAttackDist();
  _base_fld *GetFld();
  void NextPass();
  void SetAccumulationCountAdd(int nTempAccumulationCount);
  __int64 GetAccumulationCount();
};

int _Check_SF_UseType(_base_fld *pFld, int nEffectType);


