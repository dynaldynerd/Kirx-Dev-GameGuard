#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"
#include "CLootingMgr.h"
#include "CMonsterAggroMgr.h"
#include "CMonsterHierarchy.h"
#include "CMonsterSkillPool.h"
#include "CMonsterAI.h"
#include "CLuaSignalReActor.h"

class CPlayer;
class CMonsterSkill;
class CMonsterAttack;
class CGameObject;
struct _monster_create_setdata;
struct _skill_fld;
struct _force_fld;
struct _event_set;

/* 1635 */
class __cppobj CMonster : public CCharacter
{
public:
  bool m_bOper;
  bool m_bApparition;
  bool m_bDungeon;
  unsigned int m_dwLastDestroyTime;
  unsigned int m_dwDestroyNextTime;
  unsigned int m_dwLastRecoverTime;
  float m_fCreatePos[3];
  float m_fLookAtPos[3];
  bool m_bRobExp;
  bool m_bRewardExp;
  bool m_bStdItemLoot;
  _mon_active *m_pActiveRec;
  _monster_fld *m_pMonRec;
  _dummy_position *m_pDumPosition;
  int m_nHP;
  CLootingMgr m_LootMgr;
  CMonsterAggroMgr m_AggroMgr;
  CMonsterHierarchy m_MonHierarcy;
  MonsterSFContDamageToleracne m_SFContDamageTolerance;
  unsigned __int8 m_byCreateDate[4];
  unsigned int m_LifeMax;
  unsigned int m_LifeCicle;
  $B1473DED9FF3433080ADDC1400B1BAA4 ___u23;
  EmotionPresentationChecker m_EmotionPresentationCheck;
  float m_fYAngle;
  float m_fStartLookAtPos[3];
  bool m_bRotateMonster;
  MonsterStateData m_MonsterStateData;
  MonsterStateData m_BeforeMonsterStateData;
  CCharacter *m_pTargetChar;
  CMonsterSkillPool m_MonsterSkillPool;
  int m_DefPart[5];
  int m_nEventItemNum;
  _event_loot_item m_eventItem[16];
  _event_respawn *m_pEventRespawn;
  _event_set *m_pEventSet;
  CMonsterAI m_AI;
  CLuaSignalReActor m_LuaSignalReActor;

  void Init(_object_id *pID);
  char Create(_monster_create_setdata *pData);
  bool Destroy(unsigned __int8 byDestroyCode, CPlayer *pAttObj);
  float GetYAngle();
  unsigned __int8 GetYAngleByte();
  bool IsRoateMonster();
  void UpdateLookAtPos();
  void UpdateLookAtPos(float *vLookAt);
  float GetVisualField();
  float GetVisualAngle();
  bool IsViewArea(CCharacter *pTarget);
  bool GetViewAngleCap(int nCapKind, int *nOutValue);
  __int64 GetMob_AsistType();
  __int64 GetMob_SubRace();
  unsigned int GetAggroShortTime();
  unsigned int GetAggroResetTime();
  static unsigned int GetNewMonSerial();
  __int64 GetMonsterGrade();
  unsigned __int8 GetMoveType();
  unsigned __int8 GetCombatState();
  unsigned __int8 GetEmotionState();
  char CheckMonsterStateData();
  float GetMoveSpeed();
  void SetMoveType(unsigned __int8 bMoveType);
  void SetCombatState(unsigned __int8 byCombatState);
  void SetEmotionState(unsigned __int8 byEmotionState);
  unsigned __int16 GetMonStateInfo();
  __int64 GetAttackPart();
  __int64 GetAttackDP();
  float GetAttackRange();
  __int64 GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart) override;
  float GetDefFacing(int nPart) override;
  float GetDefGap(int nPart) override;
  __int64 GetDefSkill(bool bBackAttackDamage) override;
  __int64 GetFireTol() override;
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
  __int64 GetCritical_Exception_Rate();
  __int64 GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack);
  int IsValidPlayer();
  bool IsAttackableInTown() override;
  bool IsBeAttackedAble(bool bFirst) override;
  char IsBeDamagedAble(CCharacter *pAtter) override;
  char IsRecvableContEffect() override;
  bool IsRewardExp() override;
  __int64 GetMyDMGSFContCount();
  __int64 GetMaxDMGSFContCount();
  unsigned __int8 InsertSFContEffect(
    unsigned __int8 byContCode,
    unsigned __int8 byEffectCode,
    unsigned int dwEffectIndex,
    unsigned __int16 wDurSec,
    unsigned __int8 byLv,
    bool *pbUpMty);
  CCharacter *GetAttackTarget();
  void SetAttackTarget(CCharacter *p);
  bool IsMovable();
  CPlayer *SearchNearPlayer();
  __int64 AttackObject(int nDamage, CGameObject *pOri);
  void ChangeApparition(bool bApparition, unsigned int dwAfterKillTerm);
  void SendMsg_Change_MonsterTarget(CCharacter *pChar);
  bool CheckEventEmotionPresentation(unsigned __int8 byCheckType, CCharacter *pTarget);
  __int64 GetOffensiveType();
  __int64 GetHelpMeCase();
  bool IsPreAttackAbleMon();
  float GeEmotionImpStdTime();
  float GetSkillDelayTime(CMonsterSkill *pSkill);
  int AssistSF(CCharacter *pDst, CMonsterSkill *pskill);
  int _AssistSF_Cont_Dmg(CCharacter *pDst, CMonsterSkill *pskill);
  int _AssistSF_Cont_Support(CCharacter *pDst, CMonsterSkill *pskill);
  int _AssistSF_Cont_Temp(CCharacter *pDst, CMonsterSkill *pskill);
  __int64 Attack(CCharacter *pDst, CMonsterSkill *pskill);
  void make_gen_attack_param(CCharacter *pDst, _attack_param *pAP);
  char make_skill_attack_param(CCharacter *pDst, CMonsterSkill *pSkill, int nEffectType, _attack_param *pAP);
  void make_force_attack_param(CCharacter *pDst, CMonsterSkill *pSkill, _attack_param *pAP);
  void SendMsg_Attack_Gen(CMonsterAttack *pAT);
  void SendMsg_Attack_Skill(CMonsterAttack *pAT);
  void SendMsg_Attack_Force(CMonsterAttack *pAT);
  void SendMsg_Move();
  void SendMsg_Create();
  void SendMsg_Destroy(unsigned __int8 byDestroyCode);
  void SendMsg_Change_MonsterState();
  void SendMsg_Change_MonsterRotate();
  void SendMsg_FixPosition(int n) override;
  void SendMsg_RealMovePoint(int n) override;
  void DisableStdItemLoot();
  void LinkEventRespawn(_event_respawn *pEventRespawn);
  void LinkEventSet(_event_set *pEventSet);
  void SendMsg_Assist_Skill(
    char byErrCode,
    int nEffectCode,
    CCharacter *pDst,
    _skill_fld *pSkill_fld,
    char nSFLv);
  void SendMsg_Assist_Force(
    char byErrCode,
    CCharacter *pDst,
    _force_fld *pForc_fld,
    char nSFLv);
  void SendMsg_Emotion_Presentation(
    unsigned __int8 bylhw,
    unsigned __int16 wSubIndex,
    unsigned __int16 wRandIndex,
    unsigned int nSendTargetIndex);
  void CheckEmotionPresentation();
  void CheckLootItem(CPlayer *pOwner);
  char AddEventItem(_event_loot_item *pItem);
  char _LootItem_Std(CPlayer *pOwner);
  char _LootItem_Rwp(CPlayer *pOwner);
  char _LootItem_EventSet(CPlayer *pOwner);
  char _LootItem_Qst(CPlayer *pOwner);
  bool IsBossMonster();
  void _BossBirthWriteLog();
  void _BossDieWriteLog_Start(unsigned __int8 byDestroyCode, CPlayer *pAttObj);
  void _BossDieWriteLog_End();
  void SetDefPart(_monster_fld *pMonRec);
  __int64 CreateAI(int nType);
  void Command_ChildMonDestroy(unsigned int dwTime);
  void Loop() override;
  void OutOfSec() override;
  __int64 SetDamage(
    int nDam,
    CCharacter *pDst,
    int nDstLv,
    bool bCrt,
    int nAttackType,
    unsigned int dwAttackSerial,
    bool bJadeReturn) override;
  char SetHP(int nHP, bool bOver) override;
  void SetStun(bool bStun) override;
  CLuaSignalReActor *GetSignalReActor();

  struct _monster_loot_index
  {
    int nStartRecIndex;
    int nEndRecIndex;
  };

  static _monster_loot_index *s_idxMonsterLoot;
  static unsigned int s_dwSerialCnt;
  static int s_nLiveNum;
  static CLogFile s_logTrace_Boss_BirthAndDeath;
  static CLogFile s_logTrace_Boss_Looting;
  static void _InitSDM_LootTBL();
};

unsigned __int16 _GetMonsterContTime(unsigned __int8 byEffectCode, unsigned __int8 byLv);

CMonster *SearchEmptyMonster(bool bWithoutFail);
CMonster *CreateRespawnMonster(
  CMapData *pMap,
  unsigned __int16 wLayer,
  int nMonsterIndex,
  _mon_active *pActiveRec,
  _dummy_position *pDumPosition,
  bool bRobExp,
  bool bRewardExp,
  bool bDungeon,
  bool bWithoutFail,
  bool bApplyRopExpField);
CMonster *CreateRepMonster(
  CMapData *pMap,
  unsigned __int16 wLayer,
  float *fPos,
  char *pszMonsterCode,
  CMonster *pParent,
  bool bRobExp,
  bool bRewardExp,
  bool bDungeon,
  bool bWithoutFail,
  bool bApplyRopExpField);

