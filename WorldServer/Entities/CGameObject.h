#pragma once

#include "IdaCompat.h"

struct _base_fld;
class CMapData;
class CCharacter;
class CPlayer;
struct _object_create_setdata;
class CRect;

#include "CObjectList.h"

struct _object_id
{
  _object_id();
  _object_id(unsigned __int8 byKind, unsigned __int8 byID, unsigned __int16 wIndex);

  unsigned __int8 m_byKind;
  unsigned __int8 m_byID;
  unsigned __int16 m_wIndex;
};

struct _100_per_random_table
{
  _100_per_random_table();
  unsigned __int16 GetRand();
  void reset();

  static bool s_bRecordSet;
  static unsigned __int16 s_wRecord[10][100];

  unsigned __int16 m_wCurTable;
  unsigned __int16 m_wCurPoint;
};

class CGameObject
{
public:
	/*
  virtual ~CGameObject();

  virtual void AlterSec();
  virtual __int64 AttackableHeight();
  virtual void BeTargeted(CCharacter *pSeacher);
  CGameObject();
  void CalcAbsPos();
  __int64 CalcCirclePlayerNum(int nRange);
  __int64 CalcCirclePlayerNum(int nRange, bool (__fastcall *fnComp)(CGameObject *));
  virtual __int64 CalcCurHPRate();
  void CalcScrExtendPoint(CRect *prcWnd, CRect *prcExtend);
  void CalcScrNormalPoint(CRect *prcWnd);
  __int64 CalcSecIndex();
  void CircleReport(unsigned __int8 *pbyType, char *szMsg, unsigned __int16 nMsgSize, bool bToOne);
  void CircleReport(unsigned __int8 *pbyType, char *szMsg, unsigned __int16 nMsgSize, unsigned int dwPassObjSerial, bool bToOne);
  char Create(_object_create_setdata *pData);
  char Destroy();
  virtual bool FixTargetWhile(CCharacter *pkTarget, unsigned int dwMiliSecond);
  virtual __int64 GetAttackDP();
  virtual __int64 GetAttackLevel();
  virtual float GetAttackRange();
  virtual __int64 GetAvoidRate();
  __int64 GetCurSecNum();
  virtual __int64 GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart);
  virtual float GetDefFacing(int nPart);
  virtual float GetDefGap(int nPart);
  virtual __int64 GetDefSkill(bool bBackAttackDamage);
  virtual __int64 GetFireTol();
  virtual __int64 GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack);
  virtual __int64 GetHP();
  virtual __int64 GetLevel();
  virtual __int64 GetMaxHP();
  virtual char * GetObjName();
  virtual __int64 GetObjRace();
  virtual __int64 GetSoilTol();
  bool GetStun();
  __int64 GetUseSectorRange();
  virtual __int64 GetWaterTol();
  virtual float GetWeaponAdjust();
  virtual __int64 GetWeaponClass();
  virtual float GetWidth();
  virtual __int64 GetWindTol();
  void Init(_object_id *pID);
  virtual bool IsAttackableInTown();
  virtual bool IsBeAttackedAble(bool bFirst);
  char IsBeCirclePlayer(int nRange);
  virtual char IsBeDamagedAble(CCharacter *pAtter);
  char IsCircleObject(int nRange, CGameObject *pObject);
  virtual bool IsInTown();
  virtual char IsRecvableContEffect();
  virtual bool IsRewardExp();
  virtual bool Is_Battle_Mode();
  virtual void Loop();
  void OnLoop();
  virtual void OutOfSec();
  virtual void RecvKillMessage(CCharacter *pDier);
  __int64 RerangeSecIndex(unsigned int dwOld, unsigned int dwNew);
  void ResetSector(unsigned int dwOldSec, unsigned int dwNewSec);
  virtual bool RobbedHP(CCharacter *pDst, int nDecHP);
  virtual void SFContDelMessage(unsigned __int8 byContCode, unsigned __int8 byListIndex, bool bSend);
  virtual void SFContInsertMessage(unsigned __int8 byContCode, unsigned __int8 byListIndex, bool bAura);
  virtual void SFContUpdateTimeMessage(unsigned __int8 byContCode, unsigned __int8 byListIndex, int nLeftTime);
  virtual bool SF_AllContDamageForceRemove_Once(CCharacter *pDstObj);
  virtual bool SF_AllContHelpForceRemove_Once(CCharacter *pDstObj);
  virtual bool SF_AllContHelpSkillRemove_Once(CCharacter *pDstObj);
  virtual bool SF_AttHPtoDstFP_Once(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_ContDamageTimeInc_Once(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_ContHelpTimeInc_Once(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_ConvertMonsterTarget(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_ConvertTargetDest(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_DamageAndStun(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_FPDec(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_HPInc_Once(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_IncHPCircleParty(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_IncreaseDP(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_LateContDamageRemove_Once(CCharacter *pDstObj);
  virtual bool SF_LateContHelpForceRemove_Once(CCharacter *pDstObj);
  virtual bool SF_LateContHelpSkillRemove_Once(CCharacter *pDstObj);
  virtual bool SF_MakePortalReturnBindPositionPartyMember(CCharacter *pDstObj, float fEffectValue, unsigned __int8 *byRet);
  virtual bool SF_MakeZeroAnimusRecallTimeOnce(CCharacter *pDstObj, float fEffectValue);
  void Init(_object_id *pID);
  virtual bool SF_OthersContHelpSFRemove_Once(float fEffectValue);
  virtual bool SF_OverHealing_Once(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_RecoverAllReturnStateAnimusHPFull(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_ReleaseMonsterTarget(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_RemoveAllContHelp_Once(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_Resurrect_Once(CCharacter *pDstObj);
  virtual bool SF_ReturnBindPosition(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_SPDec(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_STInc_Once(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_SelfDestruction(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_SkillContHelpTimeInc_Once(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_Stun(CCharacter *pDstObj, float fEffectValue);
  virtual bool SF_TeleportToDestination(CCharacter *pDstObj, bool bStone);
  virtual bool SF_TransDestHP(CCharacter *pDstObj, float fEffectValue, unsigned __int8 *byRet);
  virtual bool SF_TransMonsterHP(CCharacter *pDstObj, float fEffectValue);
  void SendMsg_BreakStop();
  virtual void SendMsg_FixPosition(int n);
  virtual void SendMsg_RealFixPosition(bool bCircle);
  virtual void SendMsg_RealMovePoint(int n);
  virtual void SendMsg_SetHPInform();
  virtual void SendMsg_StunInform();
  virtual void SetAttackPart(int nAttactPart);
  void SetBreakTranspar(bool bBreak);
  char SetCurBspMap(CMapData *pMap);
  char SetCurPos(float *pPos);
  void SetCurSecNum(unsigned int dwNewSecNum);
  virtual __int64 SetDamage(int nDam, CCharacter *pDst, int nDstLv);
  virtual char SetHP(int nHP, bool bOver);
  void SetMaxVersion();
  virtual void SetStun(bool bStun);
  char UpdateSecList();
  void _ResetCirclePlayer();
  */
  void CalcAbsPos();
  __int64 CalcSecIndex();
  __int64 GetUseSectorRange();
  char Create(_object_create_setdata *pData);
  void CircleReport(unsigned __int8 *pbyType, char *szMsg, unsigned __int16 nMsgSize, bool bToOne);
  void CircleReport(
    unsigned __int8 *pbyType,
    char *szMsg,
    unsigned __int16 nMsgSize,
    unsigned int dwPassObjSerial,
    bool bToOne);
  void Init(_object_id *pID);
  bool IsInTown();

  static CGameObject *s_pSelectObject;
  static CGameObject *s_pTotalObject[42642];
  static int s_nTotalObjectNum;

  _base_fld *m_pRecordSet;
  _object_id m_ObjID;
  unsigned int m_dwObjSerial;
  bool m_bLive;
  int m_nTotalObjIndex;
  bool m_bCorpse;
  bool m_bMove;
  bool m_bStun;
  bool m_bMapLoading;
  unsigned int m_dwLastSendTime;
  float m_fCurPos[3];
  float m_fAbsPos[3];
  int m_nScreenPos[2];
  float m_fOldPos[3];
  CMapData *m_pCurMap;
  _100_per_random_table m_rtPer100;
  int m_nCirclePlayerNum;
  unsigned __int16 m_wMapLayerIndex;
  _object_list_point m_SectorPoint;
  _object_list_point m_SectorNetPoint;
  unsigned int m_dwNextFreeStunTime;
  unsigned int m_dwOldTickBreakTranspar;
  bool m_bBreakTranspar;
  bool m_bMaxVision;
  bool *m_bPlayerCircleList;
  bool m_bObserver;
  unsigned int m_dwCurSec;
};

