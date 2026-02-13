#include "pch.h"

#include "CDarkHoleChannel.h"

#include "CGameObject.h"
#include "CGuardTower.h"
#include "CItemBox.h"
#include "CMapData.h"
#include "CMonster.h"
#include "CNetProcess.h"
#include "CRecordData.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CPartyPlayer.h"
#include "DummyPosition.h"
#include "ENTER_DUNGEON_NEW_POS.h"
#include "darkhole_leader_change_inform_zocl.h"
#include "darkhole_job_count_inform_zocl.h"
#include "darkhole_outof_member_inform_zocl.h"
#include "darkhole_real_add_time_inform_zocl.h"
#include "darkhole_real_msg_inform_zocl.h"
#include "darkhole_ask_reenter_inform_zocl.h"
#include "darkhole_channel_close_inform_zocl.h"
#include "darkhole_job_pass_inform_zocl.h"
#include "darkhole_member_info_inform_zocl.h"
#include "darkhole_mission_info_inform_zocl.h"
#include "darkhole_mission_pass_inform_zocl.h"
#include "darkhole_mission_quest_inform_zocl.h"
#include "darkhole_new_member_inform_zocl.h"
#include "darkhole_new_mission_inform_zocl.h"
#include "darkhole_open_all_portal_by_result_inform_zocl.h"
#include "darkhole_open_portal_by_react_inform_zocl.h"
#include "darkhole_quest_info_inform_zocl.h"
#include "darkhole_timeout_inform_zocl.h"
#include "GlobalObjects.h"
#include "monster_fld.h"
#include "StorageList.h"

#include <mmsystem.h>

unsigned int CDarkHoleChannel::s_dwChannelSerialCounter = 0;

void _dh_player_mgr::_pos::init()
{
  pMap = nullptr;
}

_dh_player_mgr::_dh_player_mgr()
{
  Init();
}

void _dh_player_mgr::Init()
{
  pOne = nullptr;
  dwSerial = static_cast<unsigned int>(-1);
  LastPos.init();
}

bool _dh_player_mgr::IsFill()
{
  return pOne != nullptr;
}

_dh_player_mgr *CDarkHoleChannel::GetPlayerInfo(unsigned int dwSerial)
{
  for (int j = 0; j < 32; ++j)
  {
    if (m_Quester[j].IsFill() && m_Quester[j].dwSerial == dwSerial)
    {
      return &m_Quester[j];
    }
  }

  return nullptr;
}

CDarkHoleChannel::__enter_member::__enter_member()
{
  bActive = false;
  bDisnormalClose = false;
  dwDisconnectTime = 0;
}

CDarkHoleChannel::__enter_member::__enter_member(bool active, bool disnormal, unsigned int time)
{
  (void)time;
  bActive = active;
  bDisnormalClose = disnormal;
  dwDisconnectTime = timeGetTime();
}

void CDarkHoleChannel::AddMonster()
{
  _dh_mission_setup *mission = m_MissionMgr.pCurMssionPtr;
  if (!mission)
  {
    return;
  }

  for (int j = 0; j < mission->nAddMonsterNum; ++j)
  {
    __add_monster *addMonster = mission->pAddMonster[j];
    _dummy_position *dummyPos = nullptr;

    if (addMonster->ReactArea.AreaDefType == at_dummy)
    {
      dummyPos = addMonster->ReactArea.obj.dummy.pPos;
    }
    else if (addMonster->ReactArea.AreaDefType == at_block)
    {
      _dummy_position *block = addMonster->ReactArea.obj.dummy.pPos;
      dummyPos = *reinterpret_cast<_dummy_position **>(
        &block->m_szCode[8 * (rand() % *reinterpret_cast<int *>(&block->m_szCode[8])) + 16]);
    }

    if (!dummyPos)
    {
      continue;
    }

    for (int k = 0; k < addMonster->ReactObj.wNum; ++k)
    {
      float newPos[3]{};
      if (!m_pQuestSetup->pUseMap->GetRandPosInDummy(dummyPos, newPos, true))
      {
        continue;
      }

      if (addMonster->ReactObj.ObjDefType == react_kind_monster)
      {
        _monster_fld *monsterFld = addMonster->ReactObj.obj.monster.pMonsterFld;
        CreateRepMonster(
          m_pQuestSetup->pUseMap,
          m_wLayerIndex,
          newPos,
          monsterFld->m_strCode,
          nullptr,
          false,
          true,
          true,
          true,
          false);
      }
      else if (addMonster->ReactObj.ObjDefType == react_kind_mgrp)
      {
        _monster_fld *monsterFld = addMonster->ReactObj.obj.monster.pMonsterFld;
        const int count = *reinterpret_cast<int *>(&monsterFld->m_strCode[4]);
        const __int64 entry =
          *reinterpret_cast<__int64 *>(&monsterFld->m_strCode[8 * (rand() % count) + 12]);
        CreateRepMonster(
          m_pQuestSetup->pUseMap,
          m_wLayerIndex,
          newPos,
          reinterpret_cast<char *>(entry + 4),
          nullptr,
          false,
          true,
          true,
          true,
          false);
      }
    }
  }
}

void CDarkHoleChannel::ChangeMonster()
{
  _dh_mission_setup *mission = m_MissionMgr.pCurMssionPtr;
  if (!mission || mission->nChangeMonsterNum <= 0)
  {
    return;
  }

  CMonster *aliveMonsters[MAX_MONSTER]{};
  int aliveCount = 0;
  for (int j = 0; j < MAX_MONSTER; ++j)
  {
    CMonster *monster = &g_Monster[j];
    if (monster->m_bLive && monster->m_pCurMap == m_pQuestSetup->pUseMap
        && monster->m_wMapLayerIndex == m_wLayerIndex)
    {
      if (!mission->pAreaDummy
          || m_pQuestSetup->pUseMap->m_Dummy.IsInBBox(mission->pAreaDummy->m_wLineIndex, monster->m_fCurPos))
      {
        aliveMonsters[aliveCount++] = monster;
      }
    }
  }

  for (int k = 0; k < mission->nChangeMonsterNum; ++k)
  {
    __change_monster *changeMonster = mission->pChangeMonster[k];
    if (rand() % 100 > changeMonster->nProb)
    {
      continue;
    }

    _dh_mission_mgr::_if_change *missionCont = m_MissionMgr.GetMissionCont(mission);
    if (missionCont)
    {
      m_pQuestSetup->SetRealBoss(false);
      if (changeMonster->pszIfMissionDescirptCode)
      {
        if (!missionCont->pMissionPtr)
        {
          missionCont->pMissionPtr = mission;
        }
        missionCont->pszDespt = changeMonster->pszIfMissionDescirptCode;
      }
      if (changeMonster->pszifCompleteMsg)
      {
        if (!missionCont->pMissionPtr)
        {
          missionCont->pMissionPtr = mission;
        }
        missionCont->pszComMsg = changeMonster->pszifCompleteMsg;
      }
    }

    for (int m = 0; m < aliveCount; ++m)
    {
      CMonster *monster = aliveMonsters[m];
      _monster_create_setdata data{};
      memcpy_0(data.m_fStartPos, monster->m_fCreatePos, sizeof(data.m_fStartPos));
      data.m_nLayerIndex = monster->m_wMapLayerIndex;
      data.m_pMap = monster->m_pCurMap;
      data.m_pRecordSet = changeMonster->pMonsterFldB;
      data.pActiveRec = monster->m_pActiveRec;
      data.bDungeon = monster->m_bDungeon;
      data.pDumPosition = monster->m_pDumPosition;
      data.pParent = nullptr;
      monster->Destroy(1u, nullptr);
      monster->Create(&data);
    }
  }
}

void CDarkHoleChannel::ShareItemToMonster()
{
  _dh_mission_setup *mission = m_MissionMgr.pCurMssionPtr;
  if (!mission || mission->nLootItemNum <= 0)
  {
    return;
  }

  CMonster *aliveMonsters[MAX_MONSTER]{};
  int aliveCount = 0;
  for (int j = 0; j < MAX_MONSTER; ++j)
  {
    CMonster *monster = &g_Monster[j];
    if (monster->m_bLive && monster->m_pCurMap == m_pQuestSetup->pUseMap
        && monster->m_wMapLayerIndex == m_wLayerIndex)
    {
      if (!mission->pAreaDummy
          || m_pQuestSetup->pUseMap->m_Dummy.IsInBBox(mission->pAreaDummy->m_wLineIndex, monster->m_fCurPos))
      {
        aliveMonsters[aliveCount++] = monster;
      }
    }
  }

  for (int k = 0; k < mission->nLootItemNum; ++k)
  {
    __add_loot_item *loot = mission->pLootItem[k];
    if (!loot->dwDur)
    {
      continue;
    }

    CMonster *targets[MAX_MONSTER]{};
    int targetCount = 0;
    for (int m = 0; m < aliveCount; ++m)
    {
      CMonster *monster = aliveMonsters[m];
      if (loot->ReactObj.ObjDefType == react_kind_monster)
      {
        if (!loot->ReactObj.obj.monster.pMonsterFld || monster->m_pMonRec == loot->ReactObj.obj.monster.pMonsterFld)
        {
          targets[targetCount++] = monster;
        }
      }
      else if (loot->ReactObj.ObjDefType == react_kind_mgrp)
      {
        _monster_fld *monsterGroup = loot->ReactObj.obj.monster.pMonsterFld;
        bool found = false;
        for (int n = 0; n < *reinterpret_cast<int *>(&monsterGroup->m_strCode[4]); ++n)
        {
          if (*reinterpret_cast<_monster_fld **>(&monsterGroup->m_strCode[8 * n + 12]) == monster->m_pMonRec)
          {
            found = true;
            break;
          }
        }
        if (found)
        {
          targets[targetCount++] = monster;
        }
      }
    }

    if (!targetCount)
    {
      continue;
    }

    for (int j = 0; j < targetCount; ++j)
    {
      const unsigned __int16 lhs = static_cast<unsigned __int16>(rand() % targetCount);
      const unsigned __int16 rhs = static_cast<unsigned __int16>(rand() % targetCount);
      CMonster *tmp = targets[rhs];
      targets[rhs] = targets[lhs];
      targets[lhs] = tmp;
    }

    _event_loot_item item{};
    item.byItemTable = loot->byItemTableCode;
    item.wItemIndex = loot->pItemFld->m_dwIndex;

    const int quotient = static_cast<int>(loot->dwDur) / targetCount;
    const int remainder = static_cast<int>(loot->dwDur) % targetCount;

    for (int j = 0; j < quotient; ++j)
    {
      for (int m = 0; m < targetCount; ++m)
      {
        targets[m]->AddEventItem(&item);
        __trace("eventitem: mon: %s, item: %s", targets[m]->m_pMonRec->m_strCode, loot->pItemFld->m_strCode);
      }
    }

    for (int j = 0; j < remainder; ++j)
    {
      targets[j]->AddEventItem(&item);
    }
  }
}

unsigned int CDarkHoleChannel::GetLeaderSerial()
{
  if (!m_pQuestSetup)
  {
    return static_cast<unsigned int>(-1);
  }
  if (!m_pLeaderPtr)
  {
    return static_cast<unsigned int>(-1);
  }
  if (m_pLeaderPtr->IsFill())
  {
    return m_pLeaderPtr->dwSerial;
  }
  return static_cast<unsigned int>(-1);
}

bool CDarkHoleChannel::IsMoveNextMission(int nPortalIndex)
{
  if (!m_pQuestSetup)
  {
    return false;
  }
  if (m_bMoveNextMission)
  {
    return m_MissionMgr.IsOpenPortal(nPortalIndex);
  }
  return false;
}

char CDarkHoleChannel::IsAllMemberNearPosition(float *pfCenterPos, int nLen)
{
  if (!m_pQuestSetup)
  {
    return 0;
  }

  for (int j = 0; j < 32; ++j)
  {
    _dh_player_mgr *entry = &m_Quester[j];
    if (entry->IsFill() && entry->pOne->m_dwObjSerial == entry->dwSerial
        && GetSqrt(pfCenterPos, entry->pOne->m_fCurPos) > static_cast<float>(nLen))
    {
      return 0;
    }
  }
  return 1;
}

_dh_mission_setup *CDarkHoleChannel::SearchMissionFromPos(float *pfStartPos)
{
  if (!m_pQuestSetup)
  {
    return nullptr;
  }

  CMapData *useMap = m_pQuestSetup->pUseMap;
  for (int j = 0; j < m_pQuestSetup->nEmbMissionSetupNum; ++j)
  {
    _dh_mission_setup *mission = m_pQuestSetup->EmbMissionSetup[j];
    if (mission->pAreaDummy && useMap->m_Dummy.IsInBBox(mission->pAreaDummy->m_wLineIndex, pfStartPos))
    {
      return mission;
    }
  }
  return nullptr;
}

char CDarkHoleChannel::GotoNextMissionByPosition(float *pfStartPos)
{
  _dh_mission_setup *nextMission = SearchMissionFromPos(pfStartPos);
  if (!nextMission)
  {
    return 0;
  }

  m_MissionMgr.NextMission(nextMission);
  AddMonster();
  ChangeMonster();
  ShareItemToMonster();
  m_dwSendNewMissionMsgNextTime = timeGetTime() + 3000;
  m_bMoveNextMission = false;
  return 1;
}

void CDarkHoleChannel::NextMissionOtherQuester(CPlayer *pLeader, _dh_mission_setup *pNextMission)
{
  for (int j = 0; j < 32; ++j)
  {
    _dh_player_mgr *entry = &m_Quester[j];
    if (!entry->IsFill() || entry->dwSerial == pLeader->m_dwObjSerial)
    {
      continue;
    }

    float newPos[3]{};
    if (m_pQuestSetup->pUseMap->GetRandPosInDummy(pNextMission->pStartDummy, newPos, true))
    {
      entry->pOne->OutOfMap(m_pQuestSetup->pUseMap, m_wLayerIndex, 4u, newPos);
      const unsigned __int8 mapCode = static_cast<unsigned __int8>(m_pQuestSetup->pUseMap->m_pMapSet->m_dwIndex);
      entry->pOne->SendMsg_GotoRecallResult(0, mapCode, newPos, 4u);
    }
    else
    {
      entry->pOne->OutOfMap(m_pQuestSetup->pUseMap, m_wLayerIndex, 4u, pLeader->m_fCurPos);
      const unsigned __int8 mapCode = static_cast<unsigned __int8>(m_pQuestSetup->pUseMap->m_pMapSet->m_dwIndex);
      entry->pOne->SendMsg_GotoRecallResult(0, mapCode, pLeader->m_fCurPos, 4u);
    }
  }
}

void CDarkHoleChannel::SendMsg_PopMember(CPlayer *pPopMember, bool bDisconnect)
{
  _darkhole_outof_member_inform_zocl msg{};
  msg.dwMemberSerial = pPopMember->m_dwObjSerial;
  msg.bDisconnect = bDisconnect;

  unsigned __int8 type[2] = {35, 2};
  for (int j = 0; j < 32; ++j)
  {
    _dh_player_mgr *entry = &m_Quester[j];
    if (entry->IsFill())
    {
      const unsigned __int16 len = msg.size();
      g_Network.m_pProcess[0]->LoadSendMsg(
        entry->pOne->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        len);
    }
  }
}

void CDarkHoleChannel::SendMsg_LeaderChange(CPlayer *pNewLeader)
{
  (void)pNewLeader;

  _darkhole_leader_change_inform_zocl msg{};
  msg.dwNewLeaderSerial = m_dwOpenerSerial;

  unsigned __int8 type[2] = {35, 3};
  for (int j = 0; j < 32; ++j)
  {
    _dh_player_mgr *entry = &m_Quester[j];
    if (entry->IsFill())
    {
      const unsigned __int16 len = msg.size();
      g_Network.m_pProcess[0]->LoadSendMsg(
        entry->pOne->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        len);
    }
  }
}

char CDarkHoleChannel::ClearMember(CPlayer *pMember, bool bDisconnect, _dh_player_mgr::_pos *poutPlayerPos)
{
  pMember->m_pDHChannel = nullptr;

  _dh_player_mgr *target = nullptr;
  for (int j = 0; j < 32; ++j)
  {
    if (m_Quester[j].IsFill() && m_Quester[j].pOne == pMember)
    {
      target = &m_Quester[j];
      break;
    }
  }

  if (!target)
  {
    return 0;
  }

  if (poutPlayerPos)
  {
    memcpy_0(poutPlayerPos, &target->LastPos, sizeof(_dh_player_mgr::_pos));
  }

  target->Init();
  SendMsg_PopMember(pMember, bDisconnect);

  _dh_player_mgr *oldLeader = m_pLeaderPtr;
  if (m_pLeaderPtr == target)
  {
    m_pLeaderPtr = nullptr;
    unsigned int minEnterOrder = static_cast<unsigned int>(-1);
    for (int k = 0; k < 32; ++k)
    {
      if (m_Quester[k].IsFill() && minEnterOrder > static_cast<unsigned int>(m_Quester[k].nEnterOrder))
      {
        minEnterOrder = static_cast<unsigned int>(m_Quester[k].nEnterOrder);
        m_pLeaderPtr = &m_Quester[k];
      }
    }
    if (m_pLeaderPtr)
    {
      SendMsg_LeaderChange(m_pLeaderPtr->pOne);
    }
  }

  const DWORD now = timeGetTime();
  CDarkHoleChannel::__enter_member entry(0, bDisconnect, now);
  m_listEnterMember.FindNode(pMember->m_dwObjSerial, nullptr);
  m_listEnterMember.PushNode_Back(pMember->m_dwObjSerial, reinterpret_cast<char *>(&entry));
  return 1;
}

void CDarkHoleChannel::SendMsg_JobCount(unsigned __int8 nJobIndex, unsigned __int16 nCount)
{
  _darkhole_job_count_inform_zocl msg{};
  msg.byJobIndex = nJobIndex;
  msg.wJobCount = nCount;

  unsigned __int8 type[2] = {35, 6};
  for (int j = 0; j < 32; ++j)
  {
    _dh_player_mgr *entry = &m_Quester[j];
    if (entry->IsFill())
    {
      const unsigned __int16 len = msg.size();
      g_Network.m_pProcess[0]->LoadSendMsg(
        entry->pOne->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        len);
    }
  }
}

void CDarkHoleChannel::SendMsg_RealMsgInform(char *pMsg)
{
  _darkhole_real_msg_inform_zocl msg{};
  strcpy_0(msg.szMsg, pMsg);

  unsigned __int8 type[2] = {35, 18};
  for (int j = 0; j < 32; ++j)
  {
    _dh_player_mgr *entry = &m_Quester[j];
    if (entry->IsFill())
    {
      const unsigned __int16 len = msg.size();
      g_Network.m_pProcess[0]->LoadSendMsg(
        entry->pOne->m_ObjID.m_wIndex,
        type,
        msg.szMsg,
        len);
    }
  }
}

void CDarkHoleChannel::SendMsg_RealAddLimTime(int nAddSec, char *pMsg)
{
  _darkhole_real_add_time_inform_zocl msg{};
  msg.nAddSec = nAddSec;
  const __int64 limTime = m_MissionMgr.GetLimMSecTime();
  if (limTime == -1)
  {
    msg.dwTotalLimSec = static_cast<unsigned int>(-1);
  }
  else
  {
    msg.dwTotalLimSec = static_cast<unsigned int>(limTime / 1000);
  }

  if (pMsg)
  {
    strcpy_0(msg.szMsg, pMsg);
  }
  else
  {
    strcpy_0(msg.szMsg, "-1");
  }

  unsigned __int8 type[2] = {35, 19};
  for (int j = 0; j < 32; ++j)
  {
    _dh_player_mgr *entry = &m_Quester[j];
    if (entry->IsFill())
    {
      const unsigned __int16 len = msg.size();
      g_Network.m_pProcess[0]->LoadSendMsg(
        entry->pOne->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        len);
    }
  }
}

char CDarkHoleChannel::CheckEvent(
  EM_DH_EVENT eventType,
  int nContentTable,
  int nContentIndex,
  int nCount,
  CGameObject *pObj)
{
  if (!m_MissionMgr.pCurMssionPtr)
  {
    return 0;
  }
  if (m_bMoveNextMission || m_dwNextCloseTime != static_cast<unsigned int>(-1))
  {
    return 0;
  }

  char matched = 0;
  for (int nJobIndex = 0; nJobIndex < m_MissionMgr.pCurMssionPtr->nEmbJobSetupNum; ++nJobIndex)
  {
    _dh_job_setup *job = m_MissionMgr.pCurMssionPtr->EmbJobSetup[nJobIndex];
    if (m_MissionMgr.Count[nJobIndex].bPass || eventType != job->eventType)
    {
      continue;
    }

    bool jobMatched = false;
    if (job->JobSetup.pEventFld)
    {
      if (job->JobSetup.byTable == nContentTable)
      {
        jobMatched = job->JobSetup.pEventFld->m_dwIndex == nContentIndex;
      }
    }
    else
    {
      if (eventType == dh_event_hunt)
      {
        _base_fld *record = g_Main.m_tblMonster.GetRecord(nContentIndex);
        if (*reinterpret_cast<float *>(&record[25].m_strCode[60]) == 0.0f
            && *reinterpret_cast<float *>(&record[26].m_dwIndex) == 0.0f)
        {
          continue;
        }
      }
      jobMatched = true;
    }

    if (jobMatched)
    {
      m_MissionMgr.Count[nJobIndex].nCount += nCount;
      SendMsg_JobCount(
        static_cast<unsigned __int8>(nJobIndex),
        static_cast<unsigned __int16>(m_MissionMgr.Count[nJobIndex].nCount));
      matched = 1;
      break;
    }
  }

  for (int nJobIndex = 0; nJobIndex < m_MissionMgr.pCurMssionPtr->nRespondCheckNum; ++nJobIndex)
  {
    __respond_check *check = m_MissionMgr.pCurMssionPtr->pRespondCheck[nJobIndex];
    if (check->byActTableCode != static_cast<unsigned __int8>(nContentTable)
        || check->pActObjFld->m_dwIndex != nContentIndex)
    {
      continue;
    }

    if (!check->pszMsg)
    {
      _dummy_position *pPos = nullptr;
      if (check->ReactArea.AreaDefType == at_dummy)
      {
        pPos = check->ReactArea.obj.dummy.pPos;
      }
      else if (check->ReactArea.AreaDefType == at_block)
      {
        _dummy_position *block = check->ReactArea.obj.dummy.pPos;
        pPos = *reinterpret_cast<_dummy_position **>(
          &block->m_szCode[8 * (rand() % *reinterpret_cast<int *>(&block->m_szCode[8])) + 16]);
      }

      float *pStdPos = nullptr;
      if (eventType == dh_event_hunt || eventType == dh_event_take)
      {
        pStdPos = pObj->m_fCurPos;
      }

      float newPos[3]{};
      for (int spawnIndex = 0; spawnIndex < check->ReactObj.wNum; ++spawnIndex)
      {
        if (pPos)
        {
          if (!m_pQuestSetup->pUseMap->GetRandPosInDummy(pPos, newPos, true))
          {
            continue;
          }
        }
        else if (pStdPos)
        {
          m_pQuestSetup->pUseMap->GetRandPosInRange(pStdPos, 50, newPos);
        }
        else
        {
          continue;
        }

        switch (check->ReactObj.ObjDefType)
        {
          case react_kind_monster:
            CreateRepMonster(
              m_pQuestSetup->pUseMap,
              m_wLayerIndex,
              newPos,
              check->ReactObj.obj.monster.pMonsterFld->m_strCode,
              nullptr,
              false,
              true,
              true,
              true,
              false);
            break;
          case react_kind_mgrp:
          {
            _monster_fld *monsterFld = check->ReactObj.obj.monster.pMonsterFld;
            const int count = *reinterpret_cast<int *>(&monsterFld->m_strCode[4]);
            const int pick = rand() % count;
            const __int64 entry =
              *reinterpret_cast<__int64 *>(&monsterFld->m_strCode[8 * pick + 12]);
            CreateRepMonster(
              m_pQuestSetup->pUseMap,
              m_wLayerIndex,
              newPos,
              reinterpret_cast<char *>(entry + 4),
              nullptr,
              false,
              true,
              true,
              true,
              false);
            break;
          }
          case react_kind_item:
          {
            _STORAGE_LIST::_db_con item{};
            item.m_byTableCode = check->ReactObj.obj.item.byItemTableCode;
            item.m_wItemIndex = check->ReactObj.obj.item.pItemFld->m_dwIndex;
            item.m_dwDur = 1;
            item.m_dwLv = 0xFFFFFFF;
            CreateItemBox(
              &item,
              5u,
              m_pQuestSetup->pUseMap,
              m_wLayerIndex,
              newPos,
              false,
              nullptr,
              0,
              3u);
            break;
          }
        }
      }
    }
    else
    {
      SendMsg_RealMsgInform(check->pszMsg);
    }
  }

  for (int nJobIndex = 0; nJobIndex < m_MissionMgr.pCurMssionPtr->nAddSecCheckNum; ++nJobIndex)
  {
    __add_time *addTime = m_MissionMgr.pCurMssionPtr->pAddSecCheck[nJobIndex];
    bool addMatch = false;
    if (addTime->EventObj.ObjDefType == react_kind_monster)
    {
      addMatch = addTime->EventObj.obj.monster.pMonsterFld->m_dwIndex ==
        static_cast<unsigned __int16>(nContentIndex);
    }
    else if (addTime->EventObj.ObjDefType == react_kind_mgrp)
    {
      _monster_fld *monsterFld = addTime->EventObj.obj.monster.pMonsterFld;
      bool found = false;
      for (int j = 0; j < *reinterpret_cast<int *>(&monsterFld->m_strCode[4]); ++j)
      {
        if (**reinterpret_cast<int **>(&monsterFld->m_strCode[8 * j + 12]) ==
              static_cast<unsigned __int16>(nContentIndex))
        {
          found = true;
          break;
        }
      }
      addMatch = found;
    }
    else if (addTime->EventObj.ObjDefType == react_kind_item)
    {
      addMatch = addTime->EventObj.obj.item.byItemTableCode == static_cast<unsigned __int8>(nContentTable)
        && addTime->EventObj.obj.item.pItemFld->m_dwIndex == static_cast<unsigned __int16>(nContentIndex);
    }

    if (addMatch)
    {
      m_MissionMgr.nAddLimMSecTime += 1000 * addTime->dwAddSec;
      SendMsg_RealAddLimTime(addTime->dwAddSec, addTime->pszMsg);
    }
  }

  return matched;
}

void CDarkHoleChannel::Init()
{
  m_pQuestSetup = nullptr;
  m_wLayerIndex = static_cast<unsigned __int16>(-1);
  m_pLayerSet = nullptr;
  m_MissionMgr.Init();
  m_pHoleObj = nullptr;
  m_dwHoleSerial = static_cast<unsigned int>(-1);
  m_dwOpenerSerial = static_cast<unsigned int>(-1);
  for (int j = 0; j < 32; ++j)
  {
    m_Quester[j].Init();
  }
  m_pLeaderPtr = nullptr;
  m_dwEnterOrderCounter = 0;
  m_listEnterMember.ResetList();
  m_dwNextCloseTime = static_cast<unsigned int>(-1);
  m_bMoveNextMission = false;
  m_dwSendNewMissionMsgNextTime = static_cast<unsigned int>(-1);
  m_dwQuestStartTime = 0;
  m_bCheckMemberClose = false;
}

void CDarkHoleChannel::OpenDungeon(
  _dh_quest_setup *pQuestSetup,
  int nLayerIndex,
  CPlayer *pOpener,
  CDarkHole *pHoleObj)
{
  Init();
  m_pQuestSetup = pQuestSetup;
  m_wLayerIndex = static_cast<unsigned __int16>(nLayerIndex);
  m_dwOpenerSerial = pOpener->m_dwObjSerial;

  const char *name = pOpener->m_Param.GetCharNameW();
  strcpy_0(m_wszOpenerName, name);
  W2M(m_wszOpenerName, m_aszOpenerName, 0x11u);

  m_nOpenerDegree = pOpener->m_pUserDB->m_byUserDgr;
  m_nOpenerSubDegree = pOpener->m_pUserDB->m_bySubDgr;
  m_MissionMgr.pCurMssionPtr = pQuestSetup->pStartMissionSetup;
  m_dwChannelSerial = s_dwChannelSerialCounter++;
  m_pHoleObj = pHoleObj;
  m_dwHoleSerial = pHoleObj->m_dwObjSerial;

  if (pQuestSetup->bPartyOnly)
  {
    CPlayer *outMember = nullptr;
    (void)pOpener->_GetPartyMemberInCircle(&outMember, 8, 1);
    m_pPartyMng = pOpener->m_pPartyMgr;
  }

  m_pLayerSet = &pQuestSetup->pUseMap->m_ls[nLayerIndex];
  _MULTI_BLOCK *multiBlock = &pQuestSetup->pUseMap->m_mb[pQuestSetup->dwMonRepIndex];
  m_pLayerSet->ActiveLayer(multiBlock);
  CreateMonster();
  AddMonster();
  ChangeMonster();
  ShareItemToMonster();
  m_dwQuestStartTime = timeGetTime();
}

void CDarkHoleChannel::CreateMonster()
{
  if (!m_pLayerSet->IsActiveLayer())
  {
    return;
  }

  CMapData *map = m_pQuestSetup->pUseMap;
  _MULTI_BLOCK *multiBlock = m_pLayerSet->m_pMB;
  for (int j = 0; j < map->m_nMonBlockNum; ++j)
  {
    _mon_block *block = &map->m_pMonBlock[j];
    const int recordNum = multiBlock->m_ptbMonBlock[j].GetRecordNum();
    for (int k = 0; k < recordNum; ++k)
    {
      _mon_active *active = &m_pLayerSet->m_MonAct[j][k];
      _mon_active_fld *activeRec = active->m_pActRec;
      const unsigned int regenLimit = activeRec->m_dwRegenLimNum;
      if (active->m_dwCumMonNum < regenLimit
          && active->m_wMonRecIndex != 0xFFFF
          && activeRec->m_dwRegenProp > static_cast<unsigned int>(rand() % 100))
      {
        int spawnCount = static_cast<int>(regenLimit - active->m_dwCumMonNum);
        if (spawnCount < 0)
        {
          spawnCount = 0;
        }
        for (int m = 0; m < spawnCount; ++m)
        {
          const int dummyIndex = block->SelectDummyIndex();
          if (dummyIndex != -1 && block->m_pDumPos[dummyIndex]->m_bPosAble)
          {
            CreateRespawnMonster(
              map,
              m_wLayerIndex,
              active->m_wMonRecIndex,
              active,
              block->m_pDumPos[dummyIndex],
              0,
              1,
              1,
              1,
              0);
          }
        }
      }
    }
  }
}

bool CDarkHoleChannel::IsFill()
{
  return m_pQuestSetup != nullptr;
}

int CDarkHoleChannel::GetAllMemberNum()
{
  return static_cast<int>(m_listEnterMember.GetSize());
}

unsigned int CDarkHoleChannel::GetCurrentMemberNum()
{
  unsigned int count = 0;
  for (int j = 0; j < 32; ++j)
  {
    if (m_Quester[j].IsFill())
    {
      ++count;
    }
  }
  return count;
}

bool CDarkHoleChannel::CanYouEnterHole(CPlayer *pEnter)
{
  if (GetCurrentMemberNum() >= 32)
  {
    return false;
  }

  for (int j = 0; j < 32; ++j)
  {
    if (m_Quester[j].IsFill() && m_Quester[j].dwSerial == pEnter->m_dwObjSerial)
    {
      return false;
    }
  }

  if (m_pQuestSetup->bPartyOnly)
  {
    for (int k = 0; k < 8; ++k)
    {
      if (m_pPartyMng->IsPartyMember(pEnter))
      {
        return true;
      }
    }
    return pEnter->m_EP.GetEff_Have(50) > 0.0f;
  }

  if (m_dwOpenerSerial == pEnter->m_dwObjSerial)
  {
    return true;
  }
  if (m_nOpenerDegree != pEnter->m_byUserDgr)
  {
    if (pEnter->m_byUserDgr)
    {
      if (!m_nOpenerDegree)
      {
        return false;
      }
    }
    else if (m_nOpenerDegree != 2)
    {
      return false;
    }
  }
  return true;
}

bool CDarkHoleChannel::GetEnterNewPos(_ENTER_DUNGEON_NEW_POS *pNewPos)
{
  if (!m_MissionMgr.pCurMssionPtr)
  {
    return false;
  }

  pNewPos->byMapCode = m_pQuestSetup->pUseMap->m_pMapSet->m_dwIndex;
  pNewPos->wLayerIndex = m_wLayerIndex;
  _dh_mission_setup *mission = m_MissionMgr.pCurMssionPtr;
  _dh_quest_setup *questSetup = m_pQuestSetup;
  return questSetup->pUseMap->GetRandPosInDummy(mission->pStartDummy, pNewPos->fPos, true);
}

bool CDarkHoleChannel::IsReEnterable(unsigned int dwEnterSerial)
{
  __enter_member info;
  return m_listEnterMember.IsInList(dwEnterSerial, reinterpret_cast<char *>(&info)) && info.bDisnormalClose;
}

char CDarkHoleChannel::PushMember(
  CPlayer *pMember,
  bool bReconnect,
  CMapData *pOldMap,
  unsigned __int16 wLastLayer,
  float *pfOldPos)
{
  if (GetCurrentMemberNum() >= 32)
  {
    return 0;
  }

  _dh_player_mgr *slot = nullptr;
  for (int j = 0; j < 32; ++j)
  {
    if (!m_Quester[j].IsFill())
    {
      slot = &m_Quester[j];
      break;
    }
  }

  if (!slot)
  {
    return 0;
  }

  slot->pOne = pMember;
  slot->dwSerial = pMember->m_dwObjSerial;
  slot->LastPos.pMap = pOldMap;
  slot->LastPos.wLayer = wLastLayer;
  slot->LastPos.fPos[0] = pfOldPos[0];
  slot->LastPos.fPos[1] = pfOldPos[1];
  slot->LastPos.fPos[2] = pfOldPos[2];
  slot->nEnterOrder = m_dwEnterOrderCounter++;

  SendMsg_NewMember(pMember, bReconnect);
  if (m_dwOpenerSerial == pMember->m_dwObjSerial)
  {
    m_pLeaderPtr = slot;
  }
  SendMsg_LeaderChange(pMember);
  SendMsg_QuestInfo(pMember);
  SendMsg_MissionInfo(pMember);
  SendMsg_MemberInfo(pMember);

  pMember->m_pDHChannel = this;
  if (!m_listEnterMember.IsInList(pMember->m_dwObjSerial, nullptr))
  {
    __enter_member entry(true, false, 0);
    m_listEnterMember.PushNode_Back(pMember->m_dwObjSerial, reinterpret_cast<char *>(&entry));
  }
  m_bCheckMemberClose = true;
  return 1;
}

void CDarkHoleChannel::SendMsg_NewMember(CPlayer *pNewMember, bool bReconnect)
{
  _darkhole_new_member_inform_zocl msg{};
  msg.dwNewMemberSerial = pNewMember->m_dwObjSerial;
  const char *name = pNewMember->m_Param.GetCharNameW();
  strcpy_0(msg.wszNewMemberName, name);
  msg.bReconnect = bReconnect;

  unsigned __int8 type[2]{35, 1};
  for (int j = 0; j < 32; ++j)
  {
    _dh_player_mgr *entry = &m_Quester[j];
    if (entry->IsFill())
    {
      const unsigned __int16 len = msg.size();
      g_Network.m_pProcess[0]->LoadSendMsg(
        entry->pOne->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        len);
    }
  }
}

void CDarkHoleChannel::SendMsg_QuestInfo(CPlayer *pDst)
{
  _darkhole_quest_info_inform_zocl msg{};
  msg.dwLimTimeSec = static_cast<unsigned int>(-1);
  strcpy_0(msg.szDescirptCode, m_pQuestSetup->szDescirptCode);
  msg.dwPassTimeSec = (timeGetTime() - m_dwQuestStartTime) / 1000;

  _dh_reward_sub_setup *reward =
    (pDst->m_dwObjSerial == m_dwOpenerSerial) ? &m_pQuestSetup->RewardOne : &m_pQuestSetup->RewardOther;
  msg.dRwExp = reward->dExp;
  msg.dwRwPvp = reward->dwPvp;
  msg.dwRwDalant = reward->dwDalant;

  unsigned __int8 type[2]{35, 4};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(pDst->m_ObjID.m_wIndex, type, msg.szDescirptCode, len);
}

void CDarkHoleChannel::SendMsg_MissionInfo(CPlayer *pDst)
{
  _darkhole_mission_info_inform_zocl msg;
  strcpy_0(msg.szDescirptCode, m_MissionMgr.pCurMssionPtr->szDescirptCode);
  if (m_MissionMgr.GetLimMSecTime() == -1)
  {
    msg.dwLimTimeSec = static_cast<unsigned int>(-1);
  }
  else
  {
    msg.dwLimTimeSec = m_MissionMgr.GetLimMSecTime() / 1000;
  }
  msg.dwPassTimeSec = (timeGetTime() - m_MissionMgr.dwMissionStartTime) / 1000;
  msg.byOrder = m_MissionMgr.pCurMssionPtr->byJobOrder;
  msg.byJobNum = m_MissionMgr.pCurMssionPtr->nEmbJobSetupNum;

  for (int j = 0; j < m_MissionMgr.pCurMssionPtr->nEmbJobSetupNum; ++j)
  {
    _dh_job_setup *job = m_MissionMgr.pCurMssionPtr->EmbJobSetup[j];
    msg.Job[j].byType = job->eventType;
    msg.Job[j].byTableCode = job->JobSetup.byTable;
    if (job->JobSetup.pEventFld)
    {
      msg.Job[j].wRecordIndex = job->JobSetup.pEventFld->m_dwIndex;
    }
    else
    {
      msg.Job[j].wRecordIndex = static_cast<unsigned __int16>(-1);
    }
    msg.Job[j].zNeedCount = static_cast<__int16>(job->JobSetup.nEventCount);
    strcpy_0(msg.Job[j].szDescirptCode, job->szDescirptCode);
    msg.Job[j].bPass = m_MissionMgr.Count[j].bPass;
    msg.Job[j].wCurCount = static_cast<unsigned __int16>(m_MissionMgr.Count[j].nCount);
    msg.Job[j].bDisable = true;

    _dh_mission_mgr::_if_change *ifChange = m_MissionMgr.SearchCurMissionCont();
    if (ifChange && ifChange->pszDespt)
    {
      strcpy_0(msg.szDescirptCode, ifChange->pszDespt);
    }
  }

  unsigned __int8 type[2]{35, 17};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(pDst->m_ObjID.m_wIndex, type, msg.szDescirptCode, len);
}

void CDarkHoleChannel::SendMsg_MemberInfo(CPlayer *pDst)
{
  _darkhole_member_info_inform_zocl msg;
  msg.dwLeaderSerial = m_pLeaderPtr ? m_pLeaderPtr->dwSerial : static_cast<unsigned int>(-1);

  int memberCount = 0;
  for (int j = 0; j < 32; ++j)
  {
    _dh_player_mgr *entry = &m_Quester[j];
    if (entry->IsFill())
    {
      msg.List[memberCount].dwSerial = entry->dwSerial;
      const char *name = entry->pOne->m_Param.GetCharNameW();
      strcpy_0(msg.List[memberCount].wszName, name);
      ++memberCount;
    }
  }

  msg.wMemberNum = static_cast<unsigned __int16>(memberCount);
  unsigned __int8 type[2]{35, 5};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(
    pDst->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    len);
}

void CDarkHoleChannel::OnLoop()
{
  if (!IsFill())
  {
    return;
  }

  if (m_bCheckMemberClose && GetCurrentMemberNum() <= 0)
  {
    m_dwNextCloseTime = timeGetTime() - 1;
  }

  if (m_dwNextCloseTime != static_cast<unsigned int>(-1))
  {
    CloseDungeon();
    return;
  }

  if (m_bMoveNextMission)
  {
    CheckWaitNextMission();
    const DWORD now = timeGetTime();
    const int limMsTime = m_MissionMgr.GetLimMSecTime();
    if (static_cast<int>(now - m_MissionMgr.dwMissionStartTime) > limMsTime)
    {
      SendMsg_TimeOut();
      m_dwNextCloseTime = now + 3;
      return;
    }
  }
  else
  {
    CheckCurrentMission();
  }

  CheckMember();
  if (!m_bMoveNextMission && GetCurrentMemberNum() <= 0)
  {
    CloseDungeon();
    return;
  }

  CheckInnerEventDummy();
  CheckRespawnMonster();
  if (m_dwSendNewMissionMsgNextTime != static_cast<unsigned int>(-1))
  {
    CheckSendNewMissionMsg();
  }
}

void CDarkHoleChannel::CheckWaitNextMission()
{
  const DWORD now = timeGetTime();
  if (now - m_MissionMgr.dwMissionEndTime > 0x493E0)
  {
    SendMsg_TimeOut();
    m_dwNextCloseTime = now + 10000;
  }
}

void CDarkHoleChannel::CheckCurrentMission()
{
  _dh_mission_setup *currentMission = m_MissionMgr.pCurMssionPtr;
  if (!currentMission)
  {
    return;
  }

  const DWORD now = timeGetTime();
  for (int jobIndex = 0; jobIndex < currentMission->nEmbJobSetupNum; ++jobIndex)
  {
    _dh_mission_mgr::_count *jobCount = &m_MissionMgr.Count[jobIndex];
    if (jobCount->bPass)
    {
      continue;
    }

    _dh_job_setup *job = currentMission->EmbJobSetup[jobIndex];
    if (job->JobSetup.nEventCount < 1)
    {
      if (job->JobSetup.nEventCount == -1 && job->eventType == dh_event_hunt)
      {
        int liveMonsterCount = 0;
        if (job->JobSetup.pEventFld)
        {
          liveMonsterCount = static_cast<int>(GetMonsterNumInCurMissionArea(job->JobSetup.pEventFld->m_dwIndex));
        }
        else
        {
          liveMonsterCount = static_cast<int>(GetMonsterNumInCurMissionArea(-1));
        }

        if (!liveMonsterCount)
        {
          jobCount->bPass = true;
          SendMsg_JobPass(static_cast<unsigned __int8>(jobIndex));
        }
      }
    }
    else if (jobCount->nCount >= job->JobSetup.nEventCount)
    {
      jobCount->bPass = true;
      SendMsg_JobPass(static_cast<unsigned __int8>(jobIndex));
    }

    if (!jobCount->bPass)
    {
      continue;
    }

    for (int reactIndex = 0; reactIndex < job->nReactNum; ++reactIndex)
    {
      _react_sub_setup *reactSetup = job->ReactSetup[reactIndex];
      if (reactSetup->byReactType != 0)
      {
        continue;
      }

      CMapData *useMap = m_pQuestSetup->pUseMap;
      const int portalCount = (useMap->m_nPortalNum >= 128) ? 128 : useMap->m_nPortalNum;
      for (int portalIndex = 0; portalIndex < portalCount; ++portalIndex)
      {
        if (!strcmp_0(useMap->m_pPortal[portalIndex].m_pDumPos->m_szCode, reactSetup->pPortalDummy->m_szCode))
        {
          m_MissionMgr.OpenPortal(portalIndex);
          SendMsg_OpenPortalByReact(static_cast<unsigned __int16>(portalIndex));
          break;
        }
      }
    }
  }

  char missionPassed = 0;
  if (currentMission->byJobOrder == 0)
  {
    missionPassed = 1;
    for (int jobIndex = 0; jobIndex < currentMission->nEmbJobSetupNum; ++jobIndex)
    {
      if (!m_MissionMgr.Count[jobIndex].bPass)
      {
        missionPassed = 0;
        break;
      }
    }
  }
  else if (currentMission->byJobOrder == 1)
  {
    for (int jobIndex = 0; jobIndex < currentMission->nEmbJobSetupNum; ++jobIndex)
    {
      if (m_MissionMgr.Count[jobIndex].bPass)
      {
        missionPassed = 1;
        break;
      }
    }
  }

  if (missionPassed)
  {
    SendMsg_MissionPass();
    const unsigned __int8 resultType = currentMission->byResultType;
    if (resultType)
    {
      switch (resultType)
      {
        case 1u:
          m_MissionMgr.OpenPortal(-1);
          SendMsg_OpenPortalByResult(-1);
          WaitNextMission();
          ChangeMonsterApparition(5000);
          break;
        case 2u:
          GotoNextMission();
          break;
        case 0xFFu:
          WaitNextMission();
          break;
        default:
          break;
      }
    }
    else
    {
      SendMsg_QuestPass();
      _Reward();
      m_dwNextCloseTime = now + 10000;
      ChangeMonsterApparition(3000);
    }
  }
  else if (m_dwNextCloseTime == static_cast<unsigned int>(-1))
  {
    const int limMsTime = m_MissionMgr.GetLimMSecTime();
    if (static_cast<int>(now - m_MissionMgr.dwMissionStartTime) > limMsTime)
    {
      SendMsg_TimeOut();
      m_dwNextCloseTime = now + 10000;
    }
  }
}

void CDarkHoleChannel::CheckMember()
{
  for (int index = 0; index < 32; ++index)
  {
    _dh_player_mgr *entry = &m_Quester[index];
    if (!entry->IsFill())
    {
      continue;
    }

    bool shouldClear = true;
    if (entry->pOne->m_bLive && entry->pOne->m_dwObjSerial == entry->dwSerial
        && entry->pOne->m_pCurMap == m_pQuestSetup->pUseMap)
    {
      shouldClear = entry->pOne->m_wMapLayerIndex != m_wLayerIndex;
    }

    if (shouldClear)
    {
      ClearMember(entry->pOne, false, nullptr);
    }
  }
}

void CDarkHoleChannel::CloseDungeon()
{
  if (timeGetTime() < m_dwNextCloseTime)
  {
    return;
  }

  m_dwNextCloseTime = static_cast<unsigned int>(-1);
  SendMsg_ChannelClose();

  CMapData *useMap = m_pQuestSetup->pUseMap;
  const int sectorCount = useMap->m_ls[m_wLayerIndex].m_nSecNum;
  for (int sectorIndex = 0; sectorIndex < sectorCount; ++sectorIndex)
  {
    CObjectList *towerList = useMap->GetSectorListTower(m_wLayerIndex, static_cast<unsigned int>(sectorIndex));
    const int towerCount = towerList->GetSize();
    for (int towerIndex = 0; towerIndex < towerCount; ++towerIndex)
    {
      CGameObject *object = towerList->CopyItem(static_cast<unsigned int>(towerIndex));
      if (!object)
      {
        continue;
      }

      CGuardTower *tower = reinterpret_cast<CGuardTower *>(object);
      if (tower->m_pMasterTwr && tower->m_pItem)
      {
        tower->m_pMasterTwr->_TowerReturn(tower->m_pItem);
      }
    }
  }

  for (int index = 0; index < 32; ++index)
  {
    _dh_player_mgr *entry = &m_Quester[index];
    if (!entry->IsFill())
    {
      continue;
    }

    if (entry->pOne->m_bCorpse)
    {
      entry->pOne->pc_Revival(true);
    }
    else
    {
      entry->pOne->OutOfMap(entry->LastPos.pMap, 0, 4u, entry->LastPos.fPos);
      entry->pOne->SendMsg_GotoRecallResult(
        0,
        static_cast<unsigned __int8>(entry->LastPos.pMap->m_pMapSet->m_dwIndex),
        entry->LastPos.fPos,
        4u);
    }

    if (entry->pOne)
    {
      entry->pOne->m_pDHChannel = nullptr;
    }
  }

  for (int monsterIndex = 0; monsterIndex < MAX_MONSTER; ++monsterIndex)
  {
    CMonster *monster = &g_Monster[monsterIndex];
    if (monster->m_bLive && monster->m_pCurMap == useMap && monster->m_wMapLayerIndex == m_wLayerIndex)
    {
      monster->Destroy(1u, nullptr);
    }
  }

  useMap->m_ls[m_wLayerIndex].InertLayer();
  Init();
}

void CDarkHoleChannel::CheckInnerEventDummy()
{
  if (!m_pQuestSetup || !m_MissionMgr.pCurMssionPtr)
  {
    return;
  }

  const int innerCheckCount = m_MissionMgr.pCurMssionPtr->nInnerCheckNum;
  if (!innerCheckCount)
  {
    return;
  }

  for (int innerCheckIndex = 0; innerCheckIndex < innerCheckCount; ++innerCheckIndex)
  {
    if (m_MissionMgr.bInnerCheck[innerCheckIndex])
    {
      continue;
    }

    __inner_check *innerCheck = m_MissionMgr.pCurMssionPtr->pInnerCheck[innerCheckIndex];
    bool triggered = false;
    for (int memberIndex = 0; memberIndex < 32; ++memberIndex)
    {
      _dh_player_mgr *entry = &m_Quester[memberIndex];
      if (!entry->IsFill() || entry->dwSerial != entry->pOne->m_dwObjSerial || entry->pOne->GetCurSecNum() == -1
          || entry->pOne->m_pCurMap != m_pQuestSetup->pUseMap)
      {
        continue;
      }

      if (innerCheck->ReactArea_Evt.AreaDefType == at_dummy)
      {
        const _dummy_position *dummy = innerCheck->ReactArea_Evt.obj.dummy.pPos;
        triggered = m_pQuestSetup->pUseMap->m_Dummy.IsInBBox(dummy->m_wLineIndex, entry->pOne->m_fCurPos);
      }
      else if (innerCheck->ReactArea_Evt.AreaDefType == at_block)
      {
        _dummy_position *block = innerCheck->ReactArea_Evt.obj.dummy.pPos;
        const int blockCount = *reinterpret_cast<int *>(&block->m_szCode[8]);
        for (int blockIndex = 0; blockIndex < blockCount; ++blockIndex)
        {
          _dummy_position *dummy =
            *reinterpret_cast<_dummy_position **>(&block->m_szCode[8 * blockIndex + 16]);
          if (m_pQuestSetup->pUseMap->m_Dummy.IsInBBox(dummy->m_wLineIndex, entry->pOne->m_fCurPos))
          {
            triggered = true;
            break;
          }
        }
      }

      if (triggered)
      {
        break;
      }
    }

    if (!triggered)
    {
      continue;
    }

    m_MissionMgr.bInnerCheck[innerCheckIndex] = 1;
    if (innerCheck->pszMsg)
    {
      SendMsg_RealMsgInform(innerCheck->pszMsg);
      continue;
    }

    if (innerCheck->pszRespawnCode)
    {
      for (int respawnIndex = 0; respawnIndex < m_MissionMgr.nRespawnActNum; ++respawnIndex)
      {
        _dh_mission_mgr::_respawn_monster_act *respawnAct = &m_MissionMgr.RespawnMonsterAct[respawnIndex];
        if (!respawnAct->bStart
            && respawnAct->pData->bCallEvent
            && respawnAct->pData->pszDefineCode
            && !strcmp_0(respawnAct->pData->pszDefineCode, innerCheck->pszRespawnCode))
        {
          respawnAct->bStart = 1;
          break;
        }
      }
      continue;
    }

    _dummy_position *spawnDummy = nullptr;
    if (innerCheck->ReactArea_Aft.AreaDefType == at_dummy)
    {
      spawnDummy = innerCheck->ReactArea_Aft.obj.dummy.pPos;
    }
    else if (innerCheck->ReactArea_Aft.AreaDefType == at_block)
    {
      _dummy_position *block = innerCheck->ReactArea_Aft.obj.dummy.pPos;
      spawnDummy = *reinterpret_cast<_dummy_position **>(
        &block->m_szCode[8 * (rand() % *reinterpret_cast<int *>(&block->m_szCode[8])) + 16]);
    }

    if (!spawnDummy)
    {
      continue;
    }

    float spawnPos[3]{};
    for (int spawnIndex = 0; spawnIndex < innerCheck->ReactObj.wNum; ++spawnIndex)
    {
      if (!m_pQuestSetup->pUseMap->GetRandPosInDummy(spawnDummy, spawnPos, true))
      {
        continue;
      }

      switch (innerCheck->ReactObj.ObjDefType)
      {
        case react_kind_monster:
          CreateRepMonster(
            m_pQuestSetup->pUseMap,
            m_wLayerIndex,
            spawnPos,
            innerCheck->ReactObj.obj.monster.pMonsterFld->m_strCode,
            nullptr,
            false,
            true,
            true,
            true,
            false);
          break;
        case react_kind_mgrp:
        {
          _monster_fld *monsterFld = innerCheck->ReactObj.obj.monster.pMonsterFld;
          const __int64 picked = *reinterpret_cast<__int64 *>(
            &monsterFld->m_strCode[8 * (rand() % *reinterpret_cast<int *>(&monsterFld->m_strCode[4])) + 12]);
          CreateRepMonster(
            m_pQuestSetup->pUseMap,
            m_wLayerIndex,
            spawnPos,
            reinterpret_cast<char *>(picked + 4),
            nullptr,
            false,
            true,
            true,
            true,
            false);
          break;
        }
        case react_kind_item:
        {
          _STORAGE_LIST::_db_con item{};
          item.m_byTableCode = innerCheck->ReactObj.obj.item.byItemTableCode;
          item.m_wItemIndex = innerCheck->ReactObj.obj.item.pItemFld->m_dwIndex;
          item.m_dwDur = 1;
          item.m_dwLv = 0xFFFFFFF;
          CreateItemBox(&item, 5u, m_pQuestSetup->pUseMap, m_wLayerIndex, spawnPos, false, nullptr, 0, 3u);
          break;
        }
        default:
          break;
      }
    }
  }
}

void CDarkHoleChannel::CheckRespawnMonster()
{
  if (!m_pQuestSetup || !m_MissionMgr.pCurMssionPtr)
  {
    return;
  }

  const int respawnActCount = m_MissionMgr.nRespawnActNum;
  if (!respawnActCount)
  {
    return;
  }

  const DWORD now = timeGetTime();
  for (int respawnIndex = 0; respawnIndex < respawnActCount; ++respawnIndex)
  {
    _dh_mission_mgr::_respawn_monster_act *respawnAct = &m_MissionMgr.RespawnMonsterAct[respawnIndex];
    __respawn_monster *respawnData = respawnAct->pData;
    if (now - respawnAct->dwLastRespawnTime < respawnData->dwTermMSec)
    {
      continue;
    }

    respawnAct->dwLastRespawnTime = now;
    if (!respawnAct->bStart || (respawnData->nLim > 0 && respawnData->nLim <= respawnAct->nCum))
    {
      continue;
    }

    for (int slotIndex = 0; slotIndex < respawnData->ReactObj.wNum; ++slotIndex)
    {
      _dh_mission_mgr::_respawn_monster_act::_monster_data *liveSlot = &respawnAct->NowMonster[slotIndex];
      if (liveSlot->pMon && liveSlot->pMon->m_dwObjSerial == liveSlot->dwSerial)
      {
        continue;
      }

      _dummy_position *spawnDummy = nullptr;
      if (respawnData->ReactArea.AreaDefType == at_dummy)
      {
        spawnDummy = respawnData->ReactArea.obj.dummy.pPos;
      }
      else if (respawnData->ReactArea.AreaDefType == at_block)
      {
        _dummy_position *block = respawnData->ReactArea.obj.dummy.pPos;
        spawnDummy = *reinterpret_cast<_dummy_position **>(
          &block->m_szCode[8 * (rand() % *reinterpret_cast<int *>(&block->m_szCode[8])) + 16]);
      }

      float spawnPos[3]{};
      if (!spawnDummy || !m_pQuestSetup->pUseMap->GetRandPosInDummy(spawnDummy, spawnPos, true))
      {
        continue;
      }

      if (respawnData->ReactObj.ObjDefType == react_kind_monster)
      {
        _monster_fld *monsterFld = respawnData->ReactObj.obj.monster.pMonsterFld;
        liveSlot->pMon = CreateRepMonster(
          m_pQuestSetup->pUseMap,
          m_wLayerIndex,
          spawnPos,
          monsterFld->m_strCode,
          nullptr,
          false,
          true,
          true,
          true,
          false);
      }
      else if (respawnData->ReactObj.ObjDefType == react_kind_mgrp)
      {
        _monster_fld *monsterFld = respawnData->ReactObj.obj.monster.pMonsterFld;
        const __int64 picked = *reinterpret_cast<__int64 *>(
          &monsterFld->m_strCode[8 * (rand() % *reinterpret_cast<int *>(&monsterFld->m_strCode[4])) + 12]);
        liveSlot->pMon = CreateRepMonster(
          m_pQuestSetup->pUseMap,
          m_wLayerIndex,
          spawnPos,
          reinterpret_cast<char *>(picked + 4),
          nullptr,
          false,
          true,
          true,
          true,
          false);
      }

      if (liveSlot->pMon)
      {
        liveSlot->dwSerial = liveSlot->pMon->m_dwObjSerial;
        if (++respawnAct->nCum >= respawnData->nLim)
        {
          break;
        }
      }
    }
  }
}

void CDarkHoleChannel::CheckSendNewMissionMsg()
{
  const DWORD now = timeGetTime();
  if (now > m_dwSendNewMissionMsgNextTime)
  {
    m_dwSendNewMissionMsgNextTime = static_cast<unsigned int>(-1);
    SendMsg_NewMission();
  }
}

void CDarkHoleChannel::ChangeMonsterApparition(unsigned int nTermMSec)
{
  for (int monsterIndex = 0; monsterIndex < MAX_MONSTER; ++monsterIndex)
  {
    CMonster *monster = &g_Monster[monsterIndex];
    if (!monster->m_bLive || monster->m_pCurMap != m_pQuestSetup->pUseMap || monster->m_wMapLayerIndex != m_wLayerIndex
        || !monster->IsMovable())
    {
      continue;
    }

    bool inMissionArea = true;
    if (m_MissionMgr.pCurMssionPtr->pAreaDummy)
    {
      inMissionArea = m_pQuestSetup->pUseMap->m_Dummy.IsInBBox(
        m_MissionMgr.pCurMssionPtr->pAreaDummy->m_wLineIndex,
        monster->m_fCurPos);
    }

    if (!inMissionArea)
    {
      continue;
    }

    CGameObject *target = monster->SearchNearPlayer();
    if (target)
    {
      monster->AttackObject(10, target);
    }
    monster->ChangeApparition(true, nTermMSec);
  }
}

unsigned int CDarkHoleChannel::GetMonsterNumInCurMissionArea(int nMonsterRecIndex)
{
  unsigned int count = 0;
  for (int monsterIndex = 0; monsterIndex < MAX_MONSTER; ++monsterIndex)
  {
    CMonster *monster = &g_Monster[monsterIndex];
    if (!monster->m_bLive || monster->m_pCurMap != m_pQuestSetup->pUseMap || monster->m_wMapLayerIndex != m_wLayerIndex)
    {
      continue;
    }

    if (nMonsterRecIndex == -1)
    {
      if (!monster->IsMovable())
      {
        continue;
      }
    }
    else if (monster->m_pMonRec->m_dwIndex != static_cast<unsigned __int16>(nMonsterRecIndex))
    {
      continue;
    }

    if (m_MissionMgr.pCurMssionPtr->pAreaDummy)
    {
      if (m_pQuestSetup->pUseMap->m_Dummy.IsInBBox(m_MissionMgr.pCurMssionPtr->pAreaDummy->m_wLineIndex, monster->m_fCurPos))
      {
        ++count;
      }
    }
    else
    {
      ++count;
    }
  }

  return count;
}

bool CDarkHoleChannel::GotoNextMission()
{
  if (!m_MissionMgr.pCurMssionPtr)
  {
    return false;
  }

  if (m_MissionMgr.pCurMssionPtr->pNextMission)
  {
    _dh_mission_setup *nextMission = m_MissionMgr.pCurMssionPtr->pNextMission;
    m_MissionMgr.NextMission(nextMission);
    AddMonster();
    ChangeMonster();
    ShareItemToMonster();
    SendMsg_NewMission();
    m_bMoveNextMission = false;
  }

  return false;
}

void CDarkHoleChannel::WaitNextMission()
{
  m_bMoveNextMission = true;
  m_MissionMgr.dwMissionEndTime = timeGetTime();
}

bool CDarkHoleChannel::IsOpenPartyMember(CPlayer *pOpener)
{
  return m_pPartyMng->IsPartyMember(pOpener);
}

void CDarkHoleChannel::SendMsg_AskReEnter(CPlayer *pDst)
{
  _darkhole_ask_reenter_inform_zocl msg{};
  msg.wChannelIndex = m_wChannelIndex;
  msg.dwChannelSerial = m_dwChannelSerial;

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-47)};
  g_Network.m_pProcess[0]->LoadSendMsg(
    pDst->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    msg.size());
}

void CDarkHoleChannel::SendMsg_ChannelClose()
{
  _darkhole_channel_close_inform_zocl msg{};
  unsigned __int8 type[2]{35, 12};
  for (int index = 0; index < 32; ++index)
  {
    _dh_player_mgr *entry = &m_Quester[index];
    if (entry->IsFill())
    {
      g_Network.m_pProcess[0]->LoadSendMsg(entry->pOne->m_ObjID.m_wIndex, type, &msg.sDum, msg.size());
    }
  }
}

void CDarkHoleChannel::SendMsg_GateDestroy(unsigned __int8 *byType, char *pSend, unsigned __int16 nSize)
{
  for (int index = 0; index < 32; ++index)
  {
    if (m_Quester[index].IsFill())
    {
      g_Network.m_pProcess[0]->LoadSendMsg(m_Quester[index].pOne->m_ObjID.m_wIndex, byType, pSend, nSize);
    }
  }
}

void CDarkHoleChannel::SendMsg_JobPass(unsigned __int8 nJobIndex)
{
  _darkhole_job_pass_inform_zocl msg{};
  msg.byJobIndex = nJobIndex;

  unsigned __int8 type[2]{35, 9};
  for (int index = 0; index < 32; ++index)
  {
    _dh_player_mgr *entry = &m_Quester[index];
    if (entry->IsFill())
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        entry->pOne->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        msg.size());
    }
  }
}

void CDarkHoleChannel::SendMsg_MissionPass()
{
  _darkhole_mission_pass_inform_zocl msg{};
  char *completeMsg = m_MissionMgr.pCurMssionPtr->szCompleteMsg;
  _dh_mission_mgr::_if_change *missionCont = m_MissionMgr.SearchCurMissionCont();
  if (missionCont && missionCont->pszComMsg)
  {
    completeMsg = missionCont->pszComMsg;
  }
  strcpy_0(msg.szCompleteMsgCode, completeMsg);

  unsigned __int8 type[2]{35, 10};
  for (int index = 0; index < 32; ++index)
  {
    _dh_player_mgr *entry = &m_Quester[index];
    if (entry->IsFill())
    {
      g_Network.m_pProcess[0]->LoadSendMsg(entry->pOne->m_ObjID.m_wIndex, type, msg.szCompleteMsgCode, msg.size());
    }
  }
}

void CDarkHoleChannel::SendMsg_NewMission()
{
  _darkhole_new_mission_inform_zocl msg{};
  strcpy_0(msg.szDescirptCode, m_MissionMgr.pCurMssionPtr->szDescirptCode);
  if (m_MissionMgr.GetLimMSecTime() == -1)
  {
    msg.dwLimTimeSec = static_cast<unsigned int>(-1);
  }
  else
  {
    msg.dwLimTimeSec = static_cast<unsigned int>(m_MissionMgr.GetLimMSecTime() / 1000);
  }

  msg.byOrder = m_MissionMgr.pCurMssionPtr->byJobOrder;
  msg.byJobNum = m_MissionMgr.pCurMssionPtr->nEmbJobSetupNum;
  for (int jobIndex = 0; jobIndex < m_MissionMgr.pCurMssionPtr->nEmbJobSetupNum; ++jobIndex)
  {
    _dh_job_setup *job = m_MissionMgr.pCurMssionPtr->EmbJobSetup[jobIndex];
    msg.Job[jobIndex].byType = job->eventType;
    msg.Job[jobIndex].byTableCode = job->JobSetup.byTable;
    msg.Job[jobIndex].wRecordIndex = job->JobSetup.pEventFld ? job->JobSetup.pEventFld->m_dwIndex
                                                              : static_cast<unsigned __int16>(-1);
    msg.Job[jobIndex].zNeedCount = static_cast<__int16>(job->JobSetup.nEventCount);
    strcpy_0(msg.Job[jobIndex].szDescirptCode, job->szDescirptCode);
    msg.Job[jobIndex].bDisable = true;

    _dh_mission_mgr::_if_change *missionCont = m_MissionMgr.SearchCurMissionCont();
    if (missionCont && missionCont->pszDespt)
    {
      strcpy_0(msg.szDescirptCode, missionCont->pszDespt);
    }
  }

  unsigned __int8 type[2]{35, 13};
  for (int index = 0; index < 32; ++index)
  {
    _dh_player_mgr *entry = &m_Quester[index];
    if (entry->IsFill())
    {
      g_Network.m_pProcess[0]->LoadSendMsg(entry->pOne->m_ObjID.m_wIndex, type, msg.szDescirptCode, msg.size());
    }
  }
}

void CDarkHoleChannel::SendMsg_OpenPortalByReact(unsigned __int16 nPortalIndex)
{
  _darkhole_open_portal_by_react_inform_zocl msg{};
  msg.wPortalIndex = nPortalIndex;

  unsigned __int8 type[2]{35, 7};
  for (int index = 0; index < 32; ++index)
  {
    _dh_player_mgr *entry = &m_Quester[index];
    if (entry->IsFill())
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        entry->pOne->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        msg.size());
    }
  }
}

void CDarkHoleChannel::SendMsg_OpenPortalByResult(int nPortalIndex)
{
  (void)nPortalIndex;

  _darkhole_open_all_portal_by_result_inform_zocl msg{};
  unsigned __int8 type[2]{35, 8};
  if (!m_MissionMgr.pCurMssionPtr)
  {
    return;
  }

  CMapData *useMap = m_pQuestSetup->pUseMap;
  const int portalCount = (useMap->m_nPortalNum >= 128) ? 128 : useMap->m_nPortalNum;
  for (int portalIndex = 0; portalIndex < portalCount; ++portalIndex)
  {
    if (m_MissionMgr.pCurMssionPtr->facMissionPotal.find(useMap->m_pPortal[portalIndex].m_pDumPos->m_szCode))
    {
      msg.byPotalIndex[msg.byCnt++] = static_cast<unsigned __int8>(portalIndex);
    }
  }

  for (int index = 0; index < 32; ++index)
  {
    _dh_player_mgr *entry = &m_Quester[index];
    if (entry->IsFill())
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        entry->pOne->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        msg.size());
    }
  }
}

void CDarkHoleChannel::SendMsg_QuestPass()
{
  _darkhole_mission_quest_inform_zocl msg{};
  unsigned __int8 type[2]{35, 11};
  for (int index = 0; index < 32; ++index)
  {
    _dh_player_mgr *entry = &m_Quester[index];
    if (entry->IsFill())
    {
      g_Network.m_pProcess[0]->LoadSendMsg(entry->pOne->m_ObjID.m_wIndex, type, &msg.sDum, msg.size());
    }
  }
}

void CDarkHoleChannel::SendMsg_TimeOut()
{
  _darkhole_timeout_inform_zocl msg{};
  unsigned __int8 type[2]{35, 16};
  for (int index = 0; index < 32; ++index)
  {
    _dh_player_mgr *entry = &m_Quester[index];
    if (entry->IsFill())
    {
      g_Network.m_pProcess[0]->LoadSendMsg(entry->pOne->m_ObjID.m_wIndex, type, &msg.sDum, msg.size());
    }
  }
}

char CDarkHoleChannel::_Reward()
{
  _STORAGE_LIST::_db_con rewardItem{};
  int isRewarded[4]{};

  for (int memberIndex = 0; memberIndex < 32; ++memberIndex)
  {
    _dh_player_mgr *member = &m_Quester[memberIndex];
    if (!member->IsFill()
        || !member->pOne->m_bLive
        || member->pOne->m_bCorpse
        || member->pOne->m_dwObjSerial != member->dwSerial)
    {
      continue;
    }

    isRewarded[0] = 0;
    _dh_reward_sub_setup *rewardSetup = (m_dwOpenerSerial == member->dwSerial)
      ? &m_pQuestSetup->RewardOne
      : &m_pQuestSetup->RewardOther;
    member->pOne->Reward_DarkDungeon(
      rewardSetup,
      m_pQuestSetup->szQuestTitle,
      m_pQuestSetup->bRealBoss,
      &rewardItem,
      isRewarded);

    for (int notifyIndex = 0; notifyIndex < 32; ++notifyIndex)
    {
      _dh_player_mgr *notifyMember = &m_Quester[notifyIndex];
      if (!notifyMember->IsFill()
          || !notifyMember->pOne->m_bLive
          || notifyMember->pOne->m_bCorpse
          || notifyMember->pOne->m_dwObjSerial != notifyMember->dwSerial)
      {
        continue;
      }

      notifyMember->pOne->SendMsg_DarkHoleRewardMessage(&rewardItem, member->dwSerial, isRewarded[0]);
    }
  }

  return 1;
}

