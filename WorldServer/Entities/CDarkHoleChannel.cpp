#include "pch.h"

#include "CDarkHoleChannel.h"

#include "CGameObject.h"
#include "CItemBox.h"
#include "CMapData.h"
#include "CMonster.h"
#include "CRecordData.h"
#include "CPlayer.h"
#include "DummyPosition.h"
#include "darkhole_leader_change_inform_zocl.h"
#include "darkhole_job_count_inform_zocl.h"
#include "darkhole_outof_member_inform_zocl.h"
#include "darkhole_real_add_time_inform_zocl.h"
#include "darkhole_real_msg_inform_zocl.h"
#include "GlobalObjects.h"
#include "monster_fld.h"
#include "StorageList.h"

#include <mmsystem.h>

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

