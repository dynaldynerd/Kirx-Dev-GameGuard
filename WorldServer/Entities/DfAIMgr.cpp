#include "pch.h"

#include "DfAIMgr.h"

#include "CBsp.h"
#include "CMapData.h"
#include "CMonster.h"
#include "CMonsterAggroMgr.h"
#include "CMonsterAI.h"
#include "CMonsterHelper.h"
#include "CMonsterSkill.h"
#include "CMonsterSkillPool.h"
#include "CPathMgr.h"
#include "CRFMonsterAIMgr.h"
#include "EmotionType.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

#include <assert.h>
#include <crtdbg.h>
#include <cmath>
#include <cstdint>
#include <cstdlib>

DfAIMgr::DfAIMgr()
{

  m_IdentityKey = 0;
}

EMOTYPE EmotionType[5] = {};

__int64(__fastcall *DfAIMgr::ms_CheckMotiveFunction[4])(
  CMonsterSkill *pSkill,
  int nMotiveValue,
  CMonsterAI *pAI,
  CMonster *pMon,
  CCharacter **ppTar) = {
  DfAIMgr::CheckSPF_MON_MOTIVE_DF,
  DfAIMgr::CheckSPF_MON_MOTIVE_MY_HP_DOWN,
  DfAIMgr::CheckSPF_MON_MOTIVE_OTHER_HP_DOWN,
  DfAIMgr::CheckSPF_MON_MOTIVE_ATTACK_MODE_PASSAGE};

bool DfAIMgr::OnUsStateTBLInit()
{

  unsigned int stateIndex = 0;

  CRFMonsterAIMgr *aiMgr = CRFMonsterAIMgr::Instance();
  UsStateTBL *stateTable = nullptr;

  DfAIMgr *newManager = static_cast<DfAIMgr *>(operator new(0x48uLL));
  if (newManager)
  {
    new (newManager) DfAIMgr();
  }
  else
  {
    newManager = nullptr;
  }

  UsStateTBL *pObject = newManager;
  UsPoint<UsStateTBL> *stateTablePoint = &aiMgr->m_spStateTBLPoolPtr[0];
  stateTable = stateTablePoint->operator=(pObject);

  stateTable->SetInitFunction(DfAIMgr::OnDFInitHFSM);
  stateTable->SetExternCallFunction(DfAIMgr::OnDfExternCallFun);
  stateTable->Alloc(8u, 0x19u, 0x30u);

  stateTable->SetHFSMNode(1, 2u, 0x7D0u, -1, 1);
  stateTable->SetHFSMNode(2, 4u, 0x1F4u, -1, 1);
  stateTable->SetHFSMNode(7, 5u, 0x3E8u, -1, 1);

  const int randomValue = rand();
  stateTable->SetHFSMNode(6, 7u, 100 * (randomValue % 50) + 2500, -1, 1);

  stateTable->SetHFSMNode(3, 0xBu, 0x7530u, -1, 1);
  stateTable->SetHFSMNode(4, 0x10u, 0x2710u, -1, 1);
  stateTable->SetHFSMNode(5, 0x17u, 0x7530u, -1, 1);

  stateTable->Add(1u, 1u, 0, 0, DfAIMgr::Atp_SearchStart_OnLoop);
  stateTable->Add(1u, 1u, 0x1Bu, 2u, nullptr);
  stateTable->Add(1u, 2u, 0x1Au, 1u, nullptr);
  stateTable->Add(1u, 1u, 0x1Du, 0, DfAIMgr::Atp_Lost_Handler);
  stateTable->Add(1u, 2u, 0x1Du, 0, DfAIMgr::Atp_Lost_Handler);
  stateTable->Add(1u, 1u, 0x1Cu, 0, DfAIMgr::Atp_Searched_Handler);
  stateTable->Add(1u, 2u, 0x1Cu, 0, DfAIMgr::Atp_Searched_Handler);

  stateTable->Add(2u, 3u, 0, 0, DfAIMgr::Mon_SearchStart_OnLoop);
  stateTable->Add(2u, 3u, 0x1Fu, 4u, nullptr);
  stateTable->Add(2u, 4u, 0x1Eu, 3u, nullptr);
  stateTable->Add(2u, 3u, 0x20u, 0, DfAIMgr::Mon_Searched_Handler);
  stateTable->Add(2u, 4u, 0x20u, 0, DfAIMgr::Mon_Searched_Handler);

  stateTable->Add(7u, 5u, 0x22u, 6u, nullptr);
  stateTable->Add(7u, 6u, 0x21u, 5u, DfAIMgr::Mv_Stop_Handler);
  stateTable->Add(7u, 6u, 0, 0, DfAIMgr::Mv_Go_OnLoop);

  stateTable->Add(6u, 7u, 0x1Cu, 9u, DfAIMgr::Action_Change_Handler);
  stateTable->Add(6u, 8u, 0x1Cu, 9u, DfAIMgr::Action_Change_Handler);
  stateTable->Add(6u, 9u, 0x1Du, 7u, DfAIMgr::Action_Change_Handler);
  stateTable->Add(6u, 9u, 0x26u, 8u, DfAIMgr::Action_Change_Handler);
  stateTable->Add(6u, 7u, 0x26u, 8u, DfAIMgr::Action_Change_Handler);
  stateTable->Add(6u, 7u, 0, 0, DfAIMgr::Action_Wait_OnLoop);
  stateTable->Add(6u, 8u, 0, 0, DfAIMgr::Action_Patrol_OnLoop);
  stateTable->Add(6u, 9u, 0, 0, DfAIMgr::Action_Attack_OnLoop);
  stateTable->Add(6u, 0xAu, 0, 0, DfAIMgr::Action_Runaway_OnLoop);

  for (stateIndex = 11; stateIndex <= 0xF; ++stateIndex)
  {
    stateTable->Add(3u, static_cast<unsigned __int8>(stateIndex), 0, 0, DfAIMgr::Emotion_OnLoop);
  }

  stateTable->Add(3u, 0xBu, 0x23u, 0xCu, DfAIMgr::Emotion_OnChange);
  stateTable->Add(3u, 0xCu, 0x23u, 0xDu, DfAIMgr::Emotion_OnChange);
  stateTable->Add(3u, 0xDu, 0x23u, 0xEu, DfAIMgr::Emotion_OnChange);
  stateTable->Add(3u, 0xEu, 0x23u, 0xFu, DfAIMgr::Emotion_OnChange);
  stateTable->Add(3u, 0xFu, 0x24u, 0xEu, DfAIMgr::Emotion_OnChange);
  stateTable->Add(3u, 0xEu, 0x24u, 0xDu, DfAIMgr::Emotion_OnChange);
  stateTable->Add(3u, 0xDu, 0x24u, 0xCu, DfAIMgr::Emotion_OnChange);
  stateTable->Add(3u, 0xCu, 0x24u, 0xBu, DfAIMgr::Emotion_OnChange);

  for (stateIndex = 16; stateIndex <= 0x15; ++stateIndex)
  {
    stateTable->Add(4u, static_cast<unsigned __int8>(stateIndex), 0, 0, DfAIMgr::Condition_OnLoop);
  }
  for (stateIndex = 16; stateIndex <= 0x15; ++stateIndex)
  {
    stateTable->Add(4u, static_cast<unsigned __int8>(stateIndex), 0x27u, 0x10u, DfAIMgr::Condition_OnChange);
  }
  for (stateIndex = 16; stateIndex <= 0x15; ++stateIndex)
  {
    stateTable->Add(4u, static_cast<unsigned __int8>(stateIndex), 0x28u, 0x11u, DfAIMgr::Condition_OnChange);
  }
  for (stateIndex = 16; stateIndex <= 0x15; ++stateIndex)
  {
    stateTable->Add(4u, static_cast<unsigned __int8>(stateIndex), 0x29u, 0x12u, DfAIMgr::Condition_OnChange);
  }
  for (stateIndex = 16; stateIndex <= 0x15; ++stateIndex)
  {
    stateTable->Add(4u, static_cast<unsigned __int8>(stateIndex), 0x2Au, 0x13u, DfAIMgr::Condition_OnChange);
  }
  for (stateIndex = 16; stateIndex <= 0x15; ++stateIndex)
  {
    stateTable->Add(4u, static_cast<unsigned __int8>(stateIndex), 0x2Bu, 0x14u, DfAIMgr::Condition_OnChange);
  }
  for (stateIndex = 16; stateIndex <= 0x15; ++stateIndex)
  {
    stateTable->Add(4u, static_cast<unsigned __int8>(stateIndex), 0x2Cu, 0x15u, DfAIMgr::Condition_OnChange);
  }

  stateTable->Add(5u, 0x17u, 0x2Du, 0x16u, DfAIMgr::Assist_OnChange);
  stateTable->Add(5u, 0x18u, 0x2Eu, 0x17u, DfAIMgr::Assist_OnChange);
  stateTable->Add(5u, 0x16u, 0x2Eu, 0x17u, DfAIMgr::Assist_OnChange);
  stateTable->Add(5u, 0x16u, 0x2Fu, 0x18u, DfAIMgr::Assist_OnChange);
  stateTable->Add(5u, 0x17u, 0x2Fu, 0x18u, DfAIMgr::Assist_OnChange);
  stateTable->Add(5u, 0x16u, 0, 0, DfAIMgr::Assist_OnLoop);
  stateTable->Add(5u, 0x18u, 0, 0, DfAIMgr::Assist_OnLoop);

  return true;
}

int DfAIMgr::OnDFInitHFSM(UsStateTBL *pStateTBL, Us_HFSM *pHFSM)
{

  (void)pStateTBL;
  if (!pHFSM)
  {
    return 0;
  }

  CMonsterAI *ai = static_cast<CMonsterAI *>(pHFSM);
  SF_Timer *timer = ai->GetTimer(0);
  if (timer)
  {
    timer->Set(0xBB8u);
  }
  timer = ai->GetTimer(1);
  if (timer)
  {
    timer->Set(0x7D0u);
  }
  return 1;
}

void DfAIMgr::OnDfExternCallFun(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam, int nParam)
{

  if (!pHFS)
  {
    return;
  }

  Us_HFSM *hfs = pHFS;
  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive)
  {
    return;
  }

  if (dwEvent)
  {
    if (dwEvent == 1 && lpParam)
    {
      if (!mon->GetAttackTarget())
      {
        Us_FSM_Node *node = pHFS->GetNode( 5u);
        if (node->GetState() == 23)
        {
          unsigned char *paramBytes = static_cast<unsigned char *>(lpParam);
          if (!paramBytes[16] && paramBytes[17] == 1)
          {
            const int prob = *reinterpret_cast<int *>(&mon->m_pRecordSet[26].m_strCode[20]);
            const int roll = rand() % 100;
            if (prob > roll)
            {
              pHFS->SendMsg(5u, 0x2Du, lpParam);
            }
          }
        }
      }
    }
    return;
  }

  if (!lpParam)
  {
    return;
  }

  if (mon->IsBeDamagedAble(static_cast<CCharacter *>(lpParam)))
  {
    pHFS->SendMsg(5u, 0x2Eu, nullptr);
    if (mon->GetAttackTarget())
    {
      CCharacter *topAggro = mon->m_AggroMgr.GetTopAggroCharacter();
      if (!topAggro || mon->GetAttackTarget() == topAggro)
      {
        if (DfAIMgr::CheckEmotionBad(mon, static_cast<CMonsterAI *>(hfs), nParam))
        {
          hfs->SendMsg(3u, 0x23u, nullptr);
        }
      }
      else
      {
        pHFS->SendMsg(1u, 0x1Cu, topAggro);
      }
    }
    else
    {
      pHFS->SendMsg(1u, 0x1Cu, lpParam);
      mon->CheckEventEmotionPresentation( 9u, nullptr);
    }
    CMonsterHelper::HierarcyHelpCast(mon);
    hfs->SendMsg(2u, 0x1Eu, nullptr);
  }
}

void DfAIMgr::Atp_SearchStart_OnLoop(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  Us_HFSM *hfs = pHFS;
  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive || !mon->IsPreAttackAbleMon())
  {
    return;
  }

  if (!mon->m_pRecordSet)
  {
    _wassert(
      L"pMon->m_pRecordSet",
      L"G:\\00_ZoneServer_Source\\03_Temp_Source\\2009_05_13_Source_Oversea\\zoneserver\\GameMain\\NewMonster\\NewMonsterAI_Df_"
      L"Handler.cpp",
      0x92u);
  }

  const int offensiveType = mon->GetOffensiveType();
  CCharacter *target = CMonsterHelper::SearchNearPlayer(mon, offensiveType);
  if (target)
  {
    mon->CheckEventEmotionPresentation( 1u, nullptr);
    pHFS->SendMsg(1u, 0x1Cu, target);
    if (mon->GetHelpMeCase())
    {
      hfs->SendMsg(2u, 0x1Eu, nullptr);
    }
  }
}

void DfAIMgr::Atp_Lost_Handler(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  Us_HFSM *hfs = pHFS;
  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive)
  {
    return;
  }

  mon->SetAttackTarget(nullptr);
  mon->m_AggroMgr.ResetAggro();
  pHFS->SendMsg(7u, 0x21u, nullptr);
  pHFS->SendMsg(6u, 0x1Du, nullptr);

  Us_FSM_Node *parentNode = hfs[1].m_ArNode[0].m_pParent;
  if (parentNode)
  {
    const unsigned char lowByte = *reinterpret_cast<unsigned char *>(&parentNode[1].m_pParent);
    if (lowByte)
    {
      CMonster *parentMon = reinterpret_cast<CMonster *>(parentNode);
      CCharacter *target = parentMon->GetAttackTarget();
      if (target)
      {
        hfs->SendMsg(1u, 0x1Cu, target);
      }
    }
  }

  hfs->SendMsg(2u, 0x1Fu, nullptr);
}

void DfAIMgr::Atp_Searched_Handler(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;

  if (!pHFS)
  {
    return;
  }

  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive)
  {
    return;
  }

  pHFS->SendMsg(1u, 0x1Bu, nullptr);
  CCharacter *target = static_cast<CCharacter *>(lpParam);
  CCharacter *attackTarget = mon->GetAttackTarget();
  if (target && attackTarget != target)
  {
    mon->SetAttackTarget( target);
    pHFS->SendMsg(6u, 0x1Cu, nullptr);
    Us_FSM_Node *node = pHFS->GetNode( 3u);
    if (node->GetState() == 11)
    {
      pHFS->SendMsg(3u, 0x23u, nullptr);
    }
  }
}

void DfAIMgr::Mon_SearchStart_OnLoop(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  Us_HFSM *hfs = pHFS;
  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive)
  {
    return;
  }

  if (mon->GetAttackTarget())
  {
    Us_FSM_Node *node = pHFS->GetNode( 5u);
    if (node->GetState() == 23)
    {
      CMonsterHelper::HierarcyHelpCast(mon);
      const int prob = *reinterpret_cast<int *>(&mon->m_pRecordSet[26].m_strCode[16]);
      const int roll = rand() % 100;
      if (prob > roll)
      {
        const unsigned int maxCount = *reinterpret_cast<unsigned int *>(&mon->m_pRecordSet[3].m_strCode[8]);
        static _NEAR_DATA nearMon[20];
        const unsigned int found = CMonsterHelper::SearchNearMonster(mon, nearMon, 0x14u, 0);
        for (unsigned int j = 0; ; ++j)
        {
          const unsigned int count = found >= maxCount ? maxCount : found;
          if (j >= count)
          {
            break;
          }
          if (nearMon[j].pChar && nearMon[j].pChar != mon)
          {
            pHFS->SendMsg(2u, 0x20u, nearMon[j].pChar);
            _object_id *objId = &nearMon[j].pChar->m_ObjID;
            if (!objId->m_byKind && objId->m_byID == 1)
            {
              CCharacter *nearChar = nearMon[j].pChar;
              reinterpret_cast<Us_HFSM *>(&nearChar[3].m_AroundSlot[2])->SendExternMsg(1u, mon, 0);
              mon->CheckEventEmotionPresentation( 2u, nullptr);
            }
          }
        }
        const unsigned int count = found >= maxCount ? maxCount : found;
        if (count)
        {
          hfs->SendMsg(2u, 0x1Fu, nullptr);
        }
      }
    }
  }
}

void DfAIMgr::Mon_Searched_Handler(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{
  (void)pHFS;
  (void)dwEvent;
  (void)lpParam;
}

void DfAIMgr::Mv_Stop_Handler(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  Us_HFSM *hfs = pHFS;
  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive)
  {
    return;
  }

  if (mon->m_bMove || mon->m_bStun)
  {
    const float moveSpeed = mon->GetMoveSpeed();
    mon->MoveBreak(moveSpeed);
    mon->Stop();
    mon->SendMsg_BreakStop();
  }

  CPathMgr *pathFinder = static_cast<CMonsterAI *>(hfs)->GetPathFinder();
  pathFinder->Init();
}

void DfAIMgr::Mv_Go_OnLoop(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  Us_HFSM *hfs = pHFS;
  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive || mon->m_bMove)
  {
    return;
  }

  static float fNextTarPos[3];
  CPathMgr *pathFinder = static_cast<CMonsterAI *>(hfs)->GetPathFinder();
  if (pathFinder->PopNextPath(fNextTarPos))
  {
    DfAIMgr::ChangeTargetPos(mon, fNextTarPos);
  }
}

void DfAIMgr::Action_Change_Handler(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  Us_HFSM *hfs = pHFS;
  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive)
  {
    return;
  }

  Us_FSM_Node *node = pHFS->GetNode( 6u);
  if (!node)
  {
    return;
  }

  const unsigned int state = node->GetState();
  switch (state)
  {
    case 7u:
    {
      _base_fld *recordSet = mon->m_pRecordSet;
      const float baseTime = *reinterpret_cast<float *>(&recordSet[26].m_strCode[12]);
      const float randTime = static_cast<float>(50 * (rand() % 20));
      pHFS->SetLoopTime( 6, static_cast<int>(baseTime + randTime + 3000.0f));
      mon->m_AggroMgr.Init();
      mon->SetCombatState( 0);
      break;
    }
    case 8u:
    {
      const float baseTime = *reinterpret_cast<float *>(&mon->m_pRecordSet[26].m_strCode[12]) + 500.0f;
      const float randTime = static_cast<float>(50 * (rand() % 40));
      pHFS->SetLoopTime( 6, static_cast<int>(baseTime + randTime));
      mon->m_AggroMgr.Init();
      mon->SetCombatState( 0);
      break;
    }
    case 9u:
    {
      pHFS->SetLoopTime( 6, 0x1F4u);
      mon->SetCombatState( 1u);
      const unsigned int now = GetLoopTime();
      static_cast<CMonsterAI *>(hfs)->SetBattleModeTime( now);
      break;
    }
  }
}

void DfAIMgr::Action_Wait_OnLoop(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  unsigned char *objBytes = static_cast<unsigned char *>(pHFS->GetObjectA());
  if (objBytes && objBytes[24])
  {
    pHFS->SendMsg(6u, 0x26u, nullptr);
  }
}

void DfAIMgr::Action_Patrol_OnLoop(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  Us_HFSM *hfs = pHFS;
  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive)
  {
    return;
  }

  Us_FSM_Node *node = pHFS->GetNode( 1u);
  if (node->GetState() == 2)
  {
    pHFS->SendMsg(1u, 0x1Au, nullptr);
  }

  if (!mon->m_bMove && !DfAIMgr::CheckMonArea_N_ChangeState(static_cast<CMonsterAI *>(hfs), mon, 0))
  {
    if (!DfAIMgr::CheckAlienation(mon))
    {
      DfAIMgr::SearchPatrollPath(static_cast<CMonsterAI *>(hfs), mon);
    }
    hfs->SendMsg(3u, 0x24u, nullptr);
  }
}

void DfAIMgr::Action_Attack_OnLoop(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  Us_HFSM *hfs = pHFS;
  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive)
  {
    return;
  }

  CCharacter *target = mon->GetAttackTarget();
  if (!target || !target->m_bLive || target->GetStealth(true) || target->m_bCorpse || mon->m_pCurMap != target->m_pCurMap)
  {
    hfs->SendMsg(1u, 0x1Du, nullptr);
    hfs->SetLoopTime( 6, 0x1F4u);
    return;
  }

  if (DfAIMgr::CheckMonArea_N_ChangeState(static_cast<CMonsterAI *>(hfs), mon, 1))
  {
    return;
  }

  const double dist = Get3DSqrt(mon->m_fCurPos, target->m_fCurPos);
  const int lostDist = g_MonsterSetInfoData.GetLostMonsterTargetDistance();
  if (dist <= static_cast<float>(lostDist) || mon->m_AggroMgr.GetTopAggroCharacter())
  {
    if (mon->GetHelpMeCase())
    {
      Us_FSM_Node *node = pHFS->GetNode( 5u);
      if (node->GetState() == 23)
      {
        hfs->SendMsg(2u, 0x1Eu, nullptr);
      }
    }

    const unsigned int now = GetLoopTime();
    if ((DfAIMgr::CheckSPFDelayTime(static_cast<CMonsterAI *>(hfs), 1, now)
         && DfAIMgr::CheckSPF(static_cast<CMonsterAI *>(hfs), mon))
        || (DfAIMgr::CheckSPFDelayTime(static_cast<CMonsterAI *>(hfs), 0, now)
            && DfAIMgr::CheckGen(static_cast<CMonsterAI *>(hfs), mon)))
    {
      pHFS->SendMsg(7u, 0x21u, nullptr);
      const float loopTime = *reinterpret_cast<float *>(&mon->m_pRecordSet[25].m_strCode[20]) + 500.0f;
      hfs->SetLoopTime( 6, static_cast<int>(loopTime));
    }
    else
    {
      if (DfAIMgr::SearchCharacterPath(static_cast<CMonsterAI *>(hfs), mon, target))
      {
        unsigned int *hiword = reinterpret_cast<unsigned int *>(&hfs[1].m_ArNode[9].m_pParent);
        hiword[1] = 0;
      }
      else
      {
        unsigned int *hiword = reinterpret_cast<unsigned int *>(&hfs[1].m_ArNode[9].m_pParent);
        ++hiword[1];
      }
      hfs->SetLoopTime( 6, 0x1F4u);
    }
  }
  else
  {
    hfs->SendMsg(1u, 0x1Du, nullptr);
    hfs->SetLoopTime( 6, 0x1F4u);
  }
}

void DfAIMgr::Action_Runaway_OnLoop(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{
  (void)pHFS;
  (void)dwEvent;
  (void)lpParam;
}

void DfAIMgr::Emotion_OnLoop(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  Us_HFSM *hfs = pHFS;
  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive)
  {
    return;
  }

  Us_FSM_Node *node = pHFS->GetNode( 3u);
  if (node->GetState() != 12)
  {
    pHFS->SendMsg(3u, 0x24u, nullptr);
    return;
  }

  CCharacter *target = mon->GetAttackTarget();
  if (!target)
  {
    return;
  }

  const double dist = Get3DSqrt(mon->m_fCurPos, target->m_fCurPos);
  const int lostDist = g_MonsterSetInfoData.GetLostMonsterTargetDistance();
  if (dist >= static_cast<float>(lostDist))
  {
    pHFS->SendMsg(3u, 0x24u, nullptr);
    return;
  }

  const float loopTime = mon->GeEmotionImpStdTime();
  hfs->SetLoopTime( 3, static_cast<int>(loopTime));
}

void DfAIMgr::Emotion_OnChange(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  Us_HFSM *hfs = pHFS;
  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive)
  {
    return;
  }

  Us_FSM_Node *node = pHFS->GetNode( 3u);
  if (!node)
  {
    return;
  }

  const unsigned int state = node->GetState();
  switch (state)
  {
    case 0xBu:
      mon->SetMoveType( 0);
      mon->SetEmotionState( 0);
      hfs->SendMsg(1u, 0x1Du, nullptr);
      break;
    case 0xCu:
      mon->SetMoveType( 1u);
      mon->SetEmotionState( 1u);
      hfs->SetLoopTime( 3, 0x7530u);
      break;
    case 0xDu:
    {
      mon->SetMoveType( 1u);
      mon->SetEmotionState( 2u);
      const float loopTime = mon->GeEmotionImpStdTime();
      hfs->SetLoopTime( 3, static_cast<int>(loopTime));
      break;
    }
    case 0xEu:
    {
      mon->SetMoveType( 1u);
      mon->SetEmotionState( 3u);
      const float loopTime = mon->GeEmotionImpStdTime();
      hfs->SetLoopTime( 3, static_cast<int>(loopTime));
      break;
    }
    case 0xFu:
    {
      mon->SetMoveType( 1u);
      mon->SetEmotionState( 4u);
      const float loopTime = mon->GeEmotionImpStdTime();
      hfs->SetLoopTime( 3, static_cast<int>(loopTime));
      break;
    }
  }
}

void DfAIMgr::Condition_OnLoop(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  int *objectData = static_cast<int *>(pHFS->GetObjectA());
  if (!objectData)
  {
    return;
  }

  if (!reinterpret_cast<unsigned char *>(objectData)[24])
  {
    return;
  }

  const float hp = static_cast<float>(objectData[486]);
  const std::uintptr_t recordPtr = static_cast<std::uintptr_t>(reinterpret_cast<unsigned long long *>(objectData)[1]);
  const float maxHp = static_cast<float>(static_cast<int>(*reinterpret_cast<float *>(recordPtr + 1708)));
  const float hpPercent = (hp / maxHp) * 100.0f;
  if (hp == maxHp)
  {
    pHFS->SendMsg(4u, 0x27u, nullptr);
    pHFS->SetLoopTime( 4, 0x2710u);
  }
  else if (hpPercent <= *reinterpret_cast<float *>(recordPtr + 2024))
  {
    if (hpPercent <= *reinterpret_cast<float *>(recordPtr + 2028))
    {
      if (hpPercent <= *reinterpret_cast<float *>(recordPtr + 2032))
      {
        pHFS->SendMsg(4u, 0x2Bu, nullptr);
        pHFS->SetLoopTime( 4, 0x7D0u);
      }
      else
      {
        pHFS->SendMsg(4u, 0x2Au, nullptr);
        pHFS->SetLoopTime( 4, 0x2710u);
      }
    }
    else
    {
      pHFS->SendMsg(4u, 0x29u, nullptr);
      pHFS->SetLoopTime( 4, 0x2710u);
    }
  }
  else
  {
    pHFS->SendMsg(4u, 0x28u, nullptr);
    pHFS->SetLoopTime( 4, 0x2710u);
  }
}

void DfAIMgr::Condition_OnChange(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive)
  {
    return;
  }

  Us_FSM_Node *node = pHFS->GetNode( 4u);
  if (node)
  {
    const unsigned int state = node->GetState();
    if (state == 19)
    {
      mon->CheckEventEmotionPresentation( 4u, nullptr);
    }
  }
}

void DfAIMgr::Assist_OnChange(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;

  if (!pHFS)
  {
    return;
  }

  Us_HFSM *hfs = pHFS;
  CMonster *mon = static_cast<CMonster *>(pHFS->GetObjectA());
  if (!mon || !mon->m_bLive)
  {
    return;
  }

  Us_FSM_Node *node = pHFS->GetNode( 5u);
  if (!node)
  {
    return;
  }

  const unsigned int state = node->GetState();
  switch (state)
  {
    case 0x16u:
    {
      hfs[1].m_ArNode[0].m_pParent = static_cast<Us_FSM_Node *>(lpParam);
      CMonster *parentMon = reinterpret_cast<CMonster *>(hfs[1].m_ArNode[0].m_pParent);
      CCharacter *target = parentMon->GetAttackTarget();
      if (target)
      {
        pHFS->SendMsg(1u, 0x1Cu, target);
      }
      pHFS->SetLoopTime( 5, 0x7530u);
      mon->CheckEventEmotionPresentation(3u, nullptr);
      break;
    }
    case 0x17u:
      hfs[1].m_ArNode[0].m_pParent = nullptr;
      pHFS->SetLoopTime( 5, 0x7530u);
      break;
    case 0x18u:
      hfs[1].m_ArNode[0].m_pParent = nullptr;
      pHFS->SetLoopTime( 5, 0x1B58u);
      break;
  }
}

void DfAIMgr::Assist_OnLoop(Us_HFSM *pHFS, unsigned int dwEvent, void *lpParam)
{

  (void)dwEvent;
  (void)lpParam;

  if (!pHFS)
  {
    return;
  }

  unsigned char *objBytes = static_cast<unsigned char *>(pHFS->GetObjectA());
  if (objBytes && objBytes[24])
  {
    pHFS->SendMsg(5u, 0x2Eu, nullptr);
  }
}

bool DfAIMgr::CheckEmotionBad(CMonster *pMon, CMonsterAI *pAI, int nDamage)
{

  (void)nDamage;
  if (!pMon || !pAI)
  {
    return false;
  }

  if (!pMon->m_pRecordSet)
  {
    _wassert(
      L"pMon->m_pRecordSet",
      L"G:\\00_ZoneServer_Source\\03_Temp_Source\\2009_05_13_Source_Oversea\\zoneserver\\GameMain\\NewMonster\\NewMonsterAI_Df_"
      L"Handler.cpp",
      0x16Cu);
  }

  signed int normalValue = 100;
  Us_FSM_Node *node = pAI->GetNode( 3u);
  const unsigned int state = node->GetState();
  const int emoIndex = static_cast<int>(*reinterpret_cast<float *>(&pMon->m_pRecordSet[29].m_strCode[28])) - 1;
  switch (state)
  {
    case 0xBu:
      normalValue = EmotionType[emoIndex].m_Normal;
      break;
    case 0xCu:
      normalValue = EmotionType[emoIndex].m_Discomport;
      break;
    case 0xDu:
      normalValue = EmotionType[emoIndex].m_Anger;
      break;
    case 0xEu:
      normalValue = EmotionType[emoIndex].m_Fury;
      break;
    case 0xFu:
      normalValue = EmotionType[emoIndex].m_Mad;
      break;
  }

  const int roll = rand() % 100;
  return roll <= normalValue;
}

int DfAIMgr::CheckGen(CMonsterAI *pAI, CMonster *pMon)
{

  if (!pAI || !pMon)
  {
    return 0;
  }

  CCharacter *dst = pMon->GetAttackTarget();
  if (!dst)
  {
    return 0;
  }

  float dist = GetSqrt(pMon->m_fCurPos, dst->m_fCurPos);
  if (std::fabs(pMon->m_fCurPos[1] - dst->m_fCurPos[1]) > 200.0f)
  {
    return 0;
  }

  if ((dst->m_bMove && pMon->GetAttackRange() < 50.0f) || dist < 50.0f)
  {
    const float dt = R3GetLoopTime() * 15.0f;
    const float moveSpeed = pMon->GetMoveSpeed();
    dist = dist - (dt * moveSpeed) * 0.40000001f;
  }

  for (int index = 0; index < 16; ++index)
  {
    CMonsterSkill *skill = pMon->m_MonsterSkillPool.GetMonSkill(static_cast<unsigned int>(index));
    if (skill)
    {
      if (skill->IsExit() && !skill->GetType() && !skill->GetUseType())
      {
        const float delay = pMon->GetSkillDelayTime(skill);
        const float now = static_cast<float>(static_cast<int>(GetLoopTime()));
        const int beforeTime = static_cast<int>(skill->GetBeforeTime());
        if ((now - static_cast<float>(beforeTime)) >= delay)
        {
          const float attackDist = skill->GetAttackDist();
          if (attackDist >= dist)
          {
            return pMon->Attack(dst, skill);
          }
        }
      }
    }
  }
  return 0;
}

__int64 DfAIMgr::CheckSPF(CMonsterAI *pAI, CMonster *pMon)
{

  if (!pAI || !pMon)
  {
    return 0LL;
  }

  for (int index = 0; index < 16; ++index)
  {
    const int probRoll = rand() % 10000;
    CMonsterSkill *skill = pMon->m_MonsterSkillPool.GetMonSkill(static_cast<unsigned int>(index));
    if (skill)
    {
      if (skill->GetType())
      {
        const int motive = static_cast<int>(skill->GetMotive());
        if (motive < 4 && motive >= 0 && skill->IsExit())
        {
          const int accumulation = static_cast<int>(skill->GetAccumulationCount());
          const int limitCount = static_cast<int>(skill->GetSPLimitCount());
          if (accumulation < limitCount)
          {
            if (skill->GetType()
                && skill->GetSPActionProbability() < probRoll)
            {
              const unsigned int now = GetLoopTime();
              skill->Use(now, false);
            }
            else
            {
              CCharacter *target = nullptr;
              CCharacter *exceptTarget = nullptr;
              int allow = 1;
              const int motiveValue = static_cast<int>(skill->GetMotiveValue());
              if (DfAIMgr::ms_CheckMotiveFunction[motive](skill, motiveValue, pAI, pMon, &target) == 1)
              {
                const int exceptMotive = static_cast<int>(skill->GetExceptMotive());
                if (exceptMotive > 0 && exceptMotive < 4)
                {
                  const int exceptValue = static_cast<int>(skill->GetExceptMotiveValue());
                  const bool exceptOk =
                    DfAIMgr::ms_CheckMotiveFunction[exceptMotive](skill, exceptValue, pAI, pMon, &exceptTarget) == 0;
                  allow = exceptOk ? 1 : 0;
                }
                if (allow)
                {
                  if (target)
                  {
                    const float dist = Get3DSqrt(pMon->m_fCurPos, target->m_fCurPos);
                    const float attackDist = skill->GetAttackDist();
                    if (attackDist >= dist)
                    {
                      if (DfAIMgr::UseSkill_Target(pMon, target, skill))
                      {
                        return 1LL;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return 0LL;
}

int DfAIMgr::CheckSPFDelayTime(CMonsterAI *pAI, int nAttackType, unsigned int dwLoopTime)
{

  (void)dwLoopTime;
  if (!pAI)
  {
    return 0;
  }
  if (!pAI->GetObjectA())
  {
    return 0;
  }

  SF_Timer *timer = pAI->GetTimer( nAttackType);
  if (!timer)
  {
    return 0;
  }
  const unsigned int now = GetLoopTime();
  return timer->CheckTime( now);
}

__int64 DfAIMgr::CheckAlienation(CMonster *pMon)
{

  if (!pMon)
  {
    return 0LL;
  }
  if (pMon->IsRoateMonster() || pMon->m_MonHierarcy.GetParent())
  {
    return 0LL;
  }
  if (!pMon->m_pMonRec->m_nMobAlienation)
  {
    return 0LL;
  }

  const unsigned int dist =
    static_cast<unsigned int>((pMon->GetWidth() / 2.0f) + 20.0f);
  CMonster *nearMon = CMonsterHelper::SearchNearMonsterByDistance(pMon, dist);
  if (!nearMon)
  {
    return 0LL;
  }

  const float angle = GetYAngle(pMon->m_fCurPos, nearMon->m_fCurPos) + 32768.0f;
  const double rad = 6.283185307 * angle / 65535.0;
  float targetPos[3];
  float src[3];

  targetPos[0] = pMon->m_fCurPos[0] - (sin_0(rad) * static_cast<float>(static_cast<int>(dist)));
  targetPos[2] = pMon->m_fCurPos[2] - (cos_0(rad) * static_cast<float>(static_cast<int>(dist)));
  targetPos[1] = pMon->m_fCurPos[1];

  if (!pMon->m_pCurMap->m_Level.mBsp->CanYouGoThere(pMon->m_fCurPos, targetPos, &src))
  {
    src[1] = pMon->m_fCurPos[1];
    memcpy_0(targetPos, src, sizeof(targetPos));
  }

  DfAIMgr::ChangeTargetPos(pMon, targetPos);
  return 1LL;
}

__int64 DfAIMgr::CheckMonArea_N_ChangeState(CMonsterAI *pAI, CMonster *pMon, int bAttackState)
{

  if (pMon->IsRoateMonster() && !bAttackState)
  {
    const int dist = static_cast<int>(Get3DSqrt(pMon->m_fCreatePos, pMon->m_fCurPos));
    if (dist > 5)
    {
      float midPos[3];
      if (pMon->m_pCurMap->m_Level.mBsp->CanYouGoThere(
            pMon->m_fCurPos,
            pMon->m_fCreatePos,
            &midPos))
      {
        pAI->SendMsg(3u, 0x24u, nullptr);
        DfAIMgr::ChangeTargetPos(pMon, pMon->m_fCreatePos);
      }
      else
      {
        CMonsterHelper::TransPort(pMon, pMon->m_fCreatePos);
      }
      return 1LL;
    }
  }

  if (pMon->m_pMonRec->m_bMonsterCondition != 1)
  {
    if (pMon->m_MonHierarcy.GetParent())
    {
      const int guardArea = pMon->m_pMonRec->m_nGuardingArea;
      const int dist = static_cast<int>(Get3DSqrt(
        pMon->m_MonHierarcy.GetParent()->m_fCurPos,
        pMon->m_fCurPos));
      if (dist > guardArea && !bAttackState)
      {
        const float angle = GetYAngle(pMon->m_fCurPos, pMon->m_MonHierarcy.GetParent()->m_fCurPos);
        const double rad = 6.283185307 * angle / 65535.0;
        float tarPos[3];
        float midPos[3];

        tarPos[0] =
          pMon->m_fCurPos[0] - (sin_0(rad) * static_cast<float>(50 - rand() % 100 + dist));
        tarPos[2] =
          pMon->m_fCurPos[2] - (cos_0(rad) * static_cast<float>(50 - rand() % 100 + dist));
        tarPos[1] = pMon->m_fCurPos[1];

        if (pMon->m_pCurMap->m_Level.mBsp->CanYouGoThere(pMon->m_fCurPos, tarPos, &midPos))
        {
          pAI->SendMsg(3u, 0x23u, nullptr);
          const float loopTime = pMon->GeEmotionImpStdTime();
          pAI->SetLoopTime( 3, static_cast<int>(loopTime));
          DfAIMgr::ChangeTargetPos(pMon, pMon->m_MonHierarcy.GetParent()->m_fCurPos);
          return 1LL;
        }
      }
      else if (dist > 300 && bAttackState)
      {
        pMon->m_MonHierarcy.GetParent()->m_MonHierarcy.PopChildMon(pMon);
        pMon->Command_ChildMonDestroy( 0x3A98u);
        return 1LL;
      }
    }
    else if (!bAttackState)
    {
      const int dist = static_cast<int>(Get3DSqrt(pMon->m_fCreatePos, pMon->m_fCurPos));
      if (dist <= 200)
      {
        if (dist < 200 && dist >= 150)
        {
          float midPos[3];
          if (pMon->m_pCurMap->m_Level.mBsp->CanYouGoThere(
                pMon->m_fCurPos,
                pMon->m_fCreatePos,
                &midPos))
          {
            pAI->SendMsg(3u, 0x24u, nullptr);
            DfAIMgr::ChangeTargetPos(pMon, pMon->m_fCreatePos);
            return 1LL;
          }
        }
      }
      else
      {
        float midPos[3];
        if (pMon->m_pCurMap->m_Level.mBsp->CanYouGoThere(
              pMon->m_fCurPos,
              pMon->m_fCreatePos,
              &midPos))
        {
          pAI->SendMsg(3u, 0x23u, nullptr);
          const float loopTime = pMon->GeEmotionImpStdTime();
          pAI->SetLoopTime( 3, static_cast<int>(loopTime));
          DfAIMgr::ChangeTargetPos(pMon, pMon->m_fCreatePos);
          return 1LL;
        }
        if (dist > 400)
        {
          CMonsterHelper::TransPort(pMon, pMon->m_fCreatePos);
          return 1LL;
        }
      }
    }
    return 0LL;
  }

  const int minMoveArea = pMon->m_pMonRec->m_nMinMoveArea;
  const int dist = static_cast<int>(Get3DSqrt(pMon->m_fCreatePos, pMon->m_fCurPos));
  if (dist > 2 * minMoveArea)
  {
    CMonsterHelper::TransPort(pMon, pMon->m_fCreatePos);
    return 1LL;
  }
  if (dist <= minMoveArea)
  {
    return 0LL;
  }

  if (bAttackState)
  {
    pAI->SendMsg(5u, 0x2Fu, nullptr);
    CMonsterHelper::TransPort(pMon, pMon->m_fCreatePos);
    pMon->SetAttackTarget( nullptr);
    pAI->SendMsg(6u, 0x26u, nullptr);
  }
  else
  {
    pAI->SendMsg(3u, 0x23u, nullptr);
    const float loopTime = pMon->GeEmotionImpStdTime();
    pAI->SetLoopTime( 3, static_cast<int>(loopTime));
    DfAIMgr::ChangeTargetPos(pMon, pMon->m_fCreatePos);
  }
  return 1LL;
}

void DfAIMgr::SearchPatrollPath(CMonsterAI *pAI, CMonster *pMon)
{

  (void)pAI;
  static float vTargetPos_0[3];
  if (CMonsterHelper::SearchPatrolMovePos(pMon, &vTargetPos_0))
  {
    DfAIMgr::ChangeTargetPos(pMon, vTargetPos_0);
  }
}

char DfAIMgr::SearchCharacterPath(CMonsterAI *pAI, CMonster *pMon, CCharacter *pTarget)
{
  float v14[21];
  CPathMgr *pathFinder = pAI->GetPathFinder();
  if (pathFinder->GetPathSize())
  {
    if (pMon->m_bMove)
    {
      return 1;
    }
    pathFinder = pAI->GetPathFinder();
    if (!pathFinder->GetPathSize())
    {
      return 1;
    }
  }

  reinterpret_cast<unsigned char *>(&v14[8])[0] = 0;
  static float vTargetPos[3];
  if (pTarget->m_bMove)
  {
    if (!CMonsterHelper::SearchTargetMovePos_MovingTarget(pMon, pTarget, &vTargetPos))
    {
      return 0;
    }
  }
  else if (!CMonsterHelper::SearchTargetMovePos_StopTarget(pMon, pTarget, &vTargetPos))
  {
    CPathMgr *resetFinder = pAI->GetPathFinder();
    resetFinder->Init();
    return 0;
  }

  if (pMon->m_pCurMap->m_Level.mBsp->CanYouGoThere(
        pMon->m_fCurPos,
        vTargetPos,
        reinterpret_cast<float (*)[3]>(&v14[14])))
  {
    DfAIMgr::ChangeTargetPos(pMon, vTargetPos);
    CPathMgr *resetFinder = pAI->GetPathFinder();
    resetFinder->Init();
    return 1;
  }

  float pPos[3];
  CPathMgr *searchFinder = pAI->GetPathFinder();
  int pathSize = searchFinder->SearchPathA(pMon, vTargetPos, 0);
  if (pathSize >= 2 && (searchFinder = pAI->GetPathFinder(), searchFinder->PopNextPath(pPos)))
  {
    DfAIMgr::ChangeTargetPos(pMon, pPos);
    return 1;
  }

  float *tarPos = pTarget->m_fCurPos;
  CPathMgr *retryFinder = pAI->GetPathFinder();
  pathSize = retryFinder->SearchPathA(pMon, tarPos, 0);
  float pTarPos[3];
  if (pathSize >= 1 && (retryFinder = pAI->GetPathFinder(), retryFinder->PopNextPath(pTarPos)))
  {
    DfAIMgr::ChangeTargetPos(pMon, pTarPos);
    return 1;
  }

  return 0;
}

void DfAIMgr::ChangeTargetPos(CMonster *pMon, float *pTarPos)
{

  if (!pMon || !pTarPos)
  {
    return;
  }

  if (pMon->m_EP.GetEff_State(6))
  {
    return;
  }

  static float NewTar[3];
  static float fBeforeTar[3];
  memcpy_0(NewTar, pTarPos, sizeof(NewTar));
  memcpy_0(fBeforeTar, pMon->m_fTarPos, sizeof(fBeforeTar));
  if (pMon->SetTarPos(NewTar, true))
  {
    if ((pMon->m_fCurPos[0] != pMon->m_fTarPos[0]
         || pMon->m_fCurPos[1] != pMon->m_fTarPos[1]
         || pMon->m_fCurPos[2] != pMon->m_fTarPos[2])
        && (std::fabs(fBeforeTar[0] - NewTar[0]) >= 5.0f
            || std::fabs(fBeforeTar[2] - NewTar[2]) >= 5.0f))
    {
      pMon->SendMsg_Move();
    }
    pMon->m_AI.SendMsg(7u, 0x22u, nullptr);
  }
}

CCharacter *DfAIMgr::GetWisdomTarget(int nDstCaseType, CMonsterAI *pAI, CMonster *pMon)
{

  switch (nDstCaseType)
  {
    case 0:
      return pMon->GetAttackTarget();
    case 1:
      return pMon;
    case 2:
      return pAI->m_pAsistMonster;
  }
  return nullptr;
}

__int64 DfAIMgr::UseSkill_Target(CMonster *pMon, CCharacter *pTarget, CMonsterSkill *pSkill)
{

  if (!pMon || !pTarget || !pSkill)
  {
    return 0LL;
  }

  if (!pSkill->GetType())
  {
    return 0LL;
  }

  if (pSkill->GetUseType())
  {
    const int useType = static_cast<int>(pSkill->GetUseType());
    if (useType == 1 || useType == 2)
    {
      if (pMon->AssistSF(pTarget, pSkill))
      {
        return 1LL;
      }
    }
    else if (useType == 3)
    {
      if (pMon->AssistSF( pTarget, pSkill))
      {
        return 1LL;
      }
    }
  }
  else if (pMon->Attack(pTarget, pSkill))
  {
    return 1LL;
  }
  return 0LL;
}

__int64 DfAIMgr::CheckSPF_MON_MOTIVE_ATTACK_MODE_PASSAGE(
  CMonsterSkill *pSkill,
  int nMotiveValue,
  CMonsterAI *pAI,
  CMonster *pMon,
  CCharacter **ppTar)
{

  if (!pMon || !pAI || !pSkill || !ppTar)
  {
    return 0LL;
  }

  const unsigned int loopTime = GetLoopTime();
  const unsigned int battleModeTime = pAI->GetBattleModeTime();
  if (loopTime - battleModeTime <= static_cast<unsigned int>(1000 * nMotiveValue))
  {
    return 0LL;
  }

  const int dstCaseType = static_cast<int>(pSkill->GetDstCaseType());
  CCharacter *target = DfAIMgr::GetWisdomTarget(dstCaseType, pAI, pMon);
  if (!target)
  {
    return 0LL;
  }

  float dist = Get3DSqrt(pMon->m_fCurPos, target->m_fCurPos);
  if (target->m_bMove && pMon->GetAttackRange() < 50.0f)
  {
    const float dt = R3GetLoopTime() * 15.0f;
    const float moveSpeed = pMon->GetMoveSpeed();
    dist = dist - (dt * moveSpeed) * 0.40000001f;
  }

  const float delay = pMon->GetSkillDelayTime(pSkill);
  const float now = static_cast<float>(static_cast<int>(GetLoopTime()));
  const int beforeTime = static_cast<int>(pSkill->GetBeforeTime());
  if ((now - static_cast<float>(beforeTime)) < delay)
  {
    return 0LL;
  }

  const float attackDist = pSkill->GetAttackDist();
  if (attackDist < dist)
  {
    return 0LL;
  }

  *ppTar = target;
  return 1LL;
}

__int64 DfAIMgr::CheckSPF_MON_MOTIVE_MY_HP_DOWN(
  CMonsterSkill *pSkill,
  int nMotiveValue,
  CMonsterAI *pAI,
  CMonster *pMon,
  CCharacter **ppTar)
{

  if (!pMon || !pAI || !pSkill || !ppTar)
  {
    return 0LL;
  }

  const float delay = pMon->GetSkillDelayTime( pSkill);
  const float now = static_cast<float>(static_cast<int>(GetLoopTime()));
  const int beforeTime = static_cast<int>(pSkill->GetBeforeTime());
  if (delay > (now - static_cast<float>(beforeTime)))
  {
    return 0LL;
  }

  const float hp = static_cast<float>(pMon->GetHP());
  const int maxHp = pMon->GetMaxHP();
  const float hpPercent = (hp / static_cast<float>(maxHp)) * 100.0f;
  if (static_cast<float>(nMotiveValue) <= hpPercent)
  {
    return 0LL;
  }

  const int dstCaseType = static_cast<int>(pSkill->GetDstCaseType());
  CCharacter *target = DfAIMgr::GetWisdomTarget(dstCaseType, pAI, pMon);
  if (!target)
  {
    return 0LL;
  }

  *ppTar = target;
  return 1LL;
}

__int64 DfAIMgr::CheckSPF_MON_MOTIVE_OTHER_HP_DOWN(
  CMonsterSkill *pSkill,
  int nMotiveValue,
  CMonsterAI *pAI,
  CMonster *pMon,
  CCharacter **ppTar)
{

  if (!pMon || !pAI || !pSkill || !ppTar)
  {
    return 0LL;
  }

  const float delay = pMon->GetSkillDelayTime( pSkill);
  const float now = static_cast<float>(static_cast<int>(GetLoopTime()));
  const int beforeTime = static_cast<int>(pSkill->GetBeforeTime());
  if (delay > (now - static_cast<float>(beforeTime)))
  {
    return 0LL;
  }

  CMonster *assistMon = pAI->m_pAsistMonster;
  if (!assistMon)
  {
    _NEAR_DATA nearData[20];
    const unsigned int count = CMonsterHelper::SearchNearMonster(pMon, nearData, 0x14u, 1);
    for (unsigned int j = 0; j < count; ++j)
    {
      assistMon = static_cast<CMonster *>(nearData[j].pChar);
      if (assistMon && assistMon != pMon)
      {
        if (assistMon->GetMaxHP() <= 0)
        {
          return 0LL;
        }
        const float hp = static_cast<float>(assistMon->GetHP());
        const int maxHp = assistMon->GetMaxHP();
        const float hpPercent = (hp / static_cast<float>(maxHp)) * 100.0f;
        if (static_cast<float>(nMotiveValue) > hpPercent)
        {
          *ppTar = assistMon;
          return 1LL;
        }
      }
    }
    return 0LL;
  }

  if (assistMon->GetMaxHP() <= 0)
  {
    return 0LL;
  }

  const float hp = static_cast<float>(assistMon->GetHP());
  const int maxHp = assistMon->GetMaxHP();
  const float hpPercent = (hp / static_cast<float>(maxHp)) * 100.0f;
    if (static_cast<float>(nMotiveValue) > hpPercent
      && pSkill->GetDstCaseType() == 2
      && (pSkill->GetDstCaseType(),
          (*ppTar = DfAIMgr::GetWisdomTarget(static_cast<int>(pSkill->GetDstCaseType()), pAI, pMon)) != nullptr))
    {
      return 1LL;
    }

  return 0LL;
}

__int64 DfAIMgr::CheckSPF_MON_MOTIVE_DF(
  CMonsterSkill *pSkill,
  int nMotiveValue,
  CMonsterAI *pAI,
  CMonster *pMon,
  CCharacter **ppTar)
{

  (void)nMotiveValue;
  if (!pMon || !pAI || !pSkill || !ppTar)
  {
    return 0LL;
  }

  const int dstCaseType = static_cast<int>(pSkill->GetDstCaseType());
  CCharacter *target = DfAIMgr::GetWisdomTarget(dstCaseType, pAI, pMon);
  if (!target)
  {
    return 0LL;
  }

  float dist = Get3DSqrt(pMon->m_fCurPos, target->m_fCurPos);
  if (target->m_bMove && pMon->GetAttackRange() < 50.0f)
  {
    const float dt = R3GetLoopTime() * 15.0f;
    const float moveSpeed = pMon->GetMoveSpeed();
    dist = dist - (dt * moveSpeed) * 0.40000001f;
  }

  const float delay = pMon->GetSkillDelayTime( pSkill);
  const float now = static_cast<float>(static_cast<int>(GetLoopTime()));
  const int beforeTime = static_cast<int>(pSkill->GetBeforeTime());
  if ((now - static_cast<float>(beforeTime)) < delay)
  {
    return 0LL;
  }

  const float attackDist = pSkill->GetAttackDist();
  if (attackDist < dist)
  {
    return 0LL;
  }

  *ppTar = target;
  return 1LL;
}
