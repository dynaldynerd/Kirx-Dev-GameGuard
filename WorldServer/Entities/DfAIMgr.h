#pragma once

#include "CMainThread.h"

class CCharacter;
class CMonster;
class CMonsterAI;
class CMonsterSkill;

class DfAIMgr : public UsStateTBL
{
public:
  DfAIMgr();

  static bool OnUsStateTBLInit();

  static int __fastcall OnDFInitHFSM(UsStateTBL *pStateTBL, Us_HFSM *pHFSM);
  static void __fastcall OnDfExternCallFun(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData, int nDataSize);

  static void __fastcall Atp_SearchStart_OnLoop(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);
  static void __fastcall Atp_Lost_Handler(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);
  static void __fastcall Atp_Searched_Handler(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);

  static void __fastcall Mon_SearchStart_OnLoop(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);
  static void __fastcall Mon_Searched_Handler(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);

  static void __fastcall Mv_Stop_Handler(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);
  static void __fastcall Mv_Go_OnLoop(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);

  static void __fastcall Action_Change_Handler(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);
  static void __fastcall Action_Wait_OnLoop(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);
  static void __fastcall Action_Patrol_OnLoop(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);
  static void __fastcall Action_Attack_OnLoop(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);
  static void __fastcall Action_Runaway_OnLoop(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);

  static void __fastcall Emotion_OnLoop(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);
  static void __fastcall Emotion_OnChange(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);

  static void __fastcall Condition_OnLoop(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);
  static void __fastcall Condition_OnChange(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);

  static void __fastcall Assist_OnChange(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);
  static void __fastcall Assist_OnLoop(Us_HFSM *pHFSM, unsigned int dwEvent, void *pData);

  static bool __fastcall CheckEmotionBad(CMonster *pMon, CMonsterAI *pAI, int nDamage);
  static int __fastcall CheckGen(CMonsterAI *pAI, CMonster *pMon);
  static __int64 __fastcall CheckSPF(CMonsterAI *pAI, CMonster *pMon);
  static int __fastcall CheckSPFDelayTime(CMonsterAI *pAI, int nAttackType, unsigned int dwLoopTime);
  static __int64 __fastcall CheckAlienation(CMonster *pMon);
  static __int64 __fastcall CheckMonArea_N_ChangeState(CMonsterAI *pAI, CMonster *pMon, int bAttackState);
  static void __fastcall SearchPatrollPath(CMonsterAI *pAI, CMonster *pMon);
  static char __fastcall SearchCharacterPath(CMonsterAI *pAI, CMonster *pMon, CCharacter *pTarget);
  static void __fastcall ChangeTargetPos(CMonster *pMon, float *pTarPos);
  static CCharacter *__fastcall GetWisdomTarget(int nDstCaseType, CMonsterAI *pAI, CMonster *pMon);
  static __int64 __fastcall UseSkill_Target(CMonster *pMon, CCharacter *pTarget, CMonsterSkill *pSkill);

  static __int64 __fastcall CheckSPF_MON_MOTIVE_ATTACK_MODE_PASSAGE(
    CMonsterSkill *pSkill,
    int nMotiveValue,
    CMonsterAI *pAI,
    CMonster *pMon,
    CCharacter **ppTar);
  static __int64 __fastcall CheckSPF_MON_MOTIVE_MY_HP_DOWN(
    CMonsterSkill *pSkill,
    int nMotiveValue,
    CMonsterAI *pAI,
    CMonster *pMon,
    CCharacter **ppTar);
  static __int64 __fastcall CheckSPF_MON_MOTIVE_OTHER_HP_DOWN(
    CMonsterSkill *pSkill,
    int nMotiveValue,
    CMonsterAI *pAI,
    CMonster *pMon,
    CCharacter **ppTar);
  static __int64 __fastcall CheckSPF_MON_MOTIVE_DF(
    CMonsterSkill *pSkill,
    int nMotiveValue,
    CMonsterAI *pAI,
    CMonster *pMon,
    CCharacter **ppTar);

  static __int64(__fastcall *ms_CheckMotiveFunction[4])(
    CMonsterSkill *pSkill,
    int nMotiveValue,
    CMonsterAI *pAI,
    CMonster *pMon,
    CCharacter **ppTar);
};
