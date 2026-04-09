#include "pch.h"

#include "CQuestMgr.h"

#include "CRecordData.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CUserDB.h"
#include "CMapData.h"
#include "CMapOperation.h"
#include "CMgrAvatorItemHistory.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "QuestHappenEvent_fld.h"
#include "Packet/ZoneClientPacket.h"

#include <cstring>
#include <mmsystem.h>

CRecordData *CQuestMgr::s_tblQuest;
CRecordData *CQuestMgr::s_tblQuestHappenEvent;

namespace
{
  _quest_check_result s_QuestCKRet{};
  _quest_fail_result s_QuestFTRet{};
}

CQuestMgr::CQuestMgr()
  : m_pMaster(nullptr),
    m_pQuestData(nullptr),
    m_pTempHappenEvent(new _happen_event_cont[3])
{
  CQuestMgr::s_tblQuestHappenEvent = nullptr;
  CQuestMgr::s_tblQuest = nullptr;
}

CQuestMgr::~CQuestMgr()
{
  if (CQuestMgr::s_tblQuestHappenEvent)
  {
    delete[] CQuestMgr::s_tblQuestHappenEvent;
    CQuestMgr::s_tblQuestHappenEvent = nullptr;
  }

  if (CQuestMgr::s_tblQuest)
  {
    delete CQuestMgr::s_tblQuest;
    CQuestMgr::s_tblQuest = nullptr;
  }

  delete[] m_pTempHappenEvent;
  m_pTempHappenEvent = nullptr;
}

void CQuestMgr::InitMgr(CPlayer *pMaster, _QUEST_DB_BASE *pQuestData)
{
  m_pMaster = pMaster;
  m_pQuestData = pQuestData;
  m_LastHappenEvent.init();
  m_dwOldTimeoutChecktime = timeGetTime();

  for (int j = 0; j < 3; ++j)
  {
    m_pTempHappenEvent[j].init();
  }
}

void CQuestMgr::Loop()
{
  const unsigned int now = timeGetTime();
  if (now - m_dwOldTimeoutChecktime < 10000u)
  {
    return;
  }

  m_dwOldTimeoutChecktime = now;
  for (int slotIndex = 0; slotIndex < 30; ++slotIndex)
  {
    _QUEST_DB_BASE::_LIST *slot = &m_pMaster->m_Param.m_QuestDB.m_List[slotIndex];
    if (slot->byQuestType == 0xFF || slot->wIndex == 65535)
    {
      continue;
    }

    if (CQuestMgr::s_tblQuest->GetRecord(slot->wIndex))
    {
      if (slot->dwPassSec != static_cast<unsigned int>(-1))
      {
        slot->dwPassSec += 10;
        CheckFailCondition(static_cast<unsigned __int8>(slotIndex), 0, nullptr);
        if (slot->byQuestType != 255)
        {
          m_pMaster->m_pUserDB->Update_QuestUpdate(static_cast<unsigned __int8>(slotIndex), slot, false);
        }
      }
    }
    else
    {
      DeleteQuestData(static_cast<unsigned __int8>(slotIndex));
      m_pMaster->m_pUserDB->Update_QuestDelete(static_cast<unsigned __int8>(slotIndex));
    }
  }
}

void CQuestMgr::CheckFailLoop(int nFailCond, char *pszCode)
{
  if (!m_pMaster)
  {
    return;
  }

  for (int j = 0; j < 30; ++j)
  {
    CheckFailCondition(static_cast<unsigned __int8>(j), nFailCond, pszCode);
  }
}

_happen_event_cont *CQuestMgr::GetLastHappenEvent()
{
  if (m_LastHappenEvent.isset())
  {
    return &m_LastHappenEvent;
  }
  return nullptr;
}

bool CQuestMgr::IsRecvedQuestByNPC(int nEventIndex)
{
  (void)nEventIndex;
  return true;// this is not a stub
}

bool CQuestMgr::IsRecvedQuestByNPC(const char *pszNPCCode)
{
  (void)pszNPCCode;
  return true; // this is not a stub
}

bool CQuestMgr::ReturnItem(char *pszItemCode, int nEndReturnItemCnt, unsigned __int8 byQuestDBSlot)
{
  (void)pszItemCode;
  (void)nEndReturnItemCnt;
  (void)byQuestDBSlot;
  return true;// this is not a stub
}

void CQuestMgr::SendMsgToMaster_ReturnItemAfterQuest(
  unsigned __int16 wItemSerial,
  unsigned __int8 byNum,
  char byQuestDBSlot)
{

  _quest_return_item_after_quest_zocl msg{};
  msg.byQuestDBSlot = byQuestDBSlot;
  msg.wSerial = wItemSerial;
  msg.byNum = byNum;

  unsigned __int8 pbyType[2]{24, 46};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_pMaster->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CQuestMgr::SendMsgToMaster_NoCompleteQuestFromNPC(char byQuestDBSlot)
{
  _quest_no_complete_from_npc_zocl msg{};
  msg.byQuestDBSlot = byQuestDBSlot;

  unsigned __int8 pbyType[2]{24, 44};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_pMaster->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CQuestMgr::SendMsgToMaster_NoHaveGiveItem(char byQuestDBSlot)
{
  _quest_no_have_give_item_zocl msg{};
  msg.byQuestDBSlot = byQuestDBSlot;

  unsigned __int8 pbyType[2]{24, 47};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_pMaster->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CQuestMgr::SendMsgToMaster_NoHaveReturnItem(char byQuestDBSlot)
{
  _quest_no_have_return_item_zocl msg{};
  msg.byQuestDBSlot = byQuestDBSlot;

  unsigned __int8 pbyType[2]{24, 48};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_pMaster->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void _quest_check_result::init()
{
  m_byCheckNum = 0;
}

bool CQuestMgr::LoadQuestData()
{
    if (!CQuestMgr::s_tblQuestHappenEvent)
    {
        CQuestMgr::s_tblQuestHappenEvent = new CRecordData[9];
    }

    const char* szFile[9];
    szFile[0] = ".\\script\\QuestDummyEvent.dat";
    szFile[1] = ".\\script\\QuestNPCEvent.dat";
    szFile[2] = ".\\script\\QuestKillOtherRaceEvent.dat";
    szFile[3] = ".\\script\\QuestLvUpEvent.dat";
    szFile[4] = ".\\script\\QuestPromoteEvent.dat";
    szFile[5] = ".\\script\\QuestGradeEvent.dat";
    szFile[6] = ".\\script\\QuestGainItemEvent.dat";
    szFile[7] = ".\\script\\QuestMasteryEvent.dat";
    szFile[8] = ".\\script\\QuestLvLimitEvent.dat";
    char pszErrMsg[160];
    for (int j = 0; j < 9; ++j)
    {
        if (!CQuestMgr::s_tblQuestHappenEvent[j].ReadRecord(
              szFile[j],
              static_cast<int>(sizeof(_QuestHappenEvent_fld)),
              pszErrMsg))
        {
            MyMessageBox("CQuestMgr Data Load", "%s Load Error", szFile[j]);
            ServerProgramExit("CQuestMgr Data Load", 0);
        }
    }

    if (!CQuestMgr::s_tblQuest)
    {
        CQuestMgr::s_tblQuest = new CRecordData();
    }
    if(!CQuestMgr::s_tblQuest->ReadRecord(".\\script\\Quest.dat", static_cast<int>(sizeof(_Quest_fld)), pszErrMsg))
    {
        MyMessageBox("CQuestMgr Data Load", "Quest.dat Load Error");
        ServerProgramExit("CQuestMgr Data Load", 0);
  }
    _Quest_fld *questRecord = 0LL;
    bool hasInvalidRewardMasteryData = false;
  for (int n = 0; ; ++n)
  {
    int RecordNum = CQuestMgr::s_tblQuest->GetRecordNum();
    if (n >= RecordNum)
      break;
        questRecord = (_Quest_fld*)CQuestMgr::s_tblQuest->GetRecord(n);
        if (questRecord)
        {
            if (!CQuestMgr::CheckRewardMasteryData(n, 0, questRecord->m_RewardMastery))
                hasInvalidRewardMasteryData = true;
            if (!CQuestMgr::CheckRewardMasteryData(n, 1, &questRecord->m_RewardMastery[1]))
                hasInvalidRewardMasteryData = true;
        }
    else
    {
      MyMessageBox("Error", "CQuestMgr::LoadQuestData() : s_tblQuest[%d] NULL!", n);
    }
  }
  if (!hasInvalidRewardMasteryData)
    return CQuestMgr::CalcStartNPCQuestCnt(g_Main.m_dwStartNPCQuestCnt);
    MyMessageBox(
        "Invalid Data",
        "Quest.dat Out of Range Reward Mastery Data!\r\nCheck LoadingProcess.log!\r\nCQuestMgr::LoadQuestData() Fail!");
    return 0;
}

bool CQuestMgr::CheckRewardMasteryData(int iInx, int iNth, _quest_reward_mastery *pData)
{
  const char *kindNames[7] =
  {
    "mastery_code_weapon",
    "mastery_code_defence",
    "mastery_code_shield",
    "mastery_code_skill",
    "mastery_code_force",
    "mastery_code_make",
    "mastery_code_special",
  };

  const int masteryId = pData->m_nConsMasteryID;
  switch (masteryId + 1)
  {
    case 0:
      return true;
    case 1:
      if (pData->m_nConsMasterySubID < 2 && pData->m_nConsMasterySubID != -1)
      {
        return true;
      }
      g_Main.m_logLoadingError.Write(
        "Quest.dat Index(%d) %dth kind(%d)%s\t\t%d\t\t%d m_RewardMastery[%d].m_nConsMasteryID(%d) >= WP_CLASS_NUM(%d) or -1 Invalid",
        iInx,
        iNth,
        pData->m_nConsMasteryID,
        kindNames[pData->m_nConsMasteryID],
        pData->m_nConsMasterySubID,
        pData->m_nConsMasteryCnt,
        iNth,
        pData->m_nConsMasterySubID,
        2);
      return false;
    case 2:
      if (pData->m_nConsMasterySubID <= 0 && pData->m_nConsMasterySubID != -1)
      {
        return true;
      }
      g_Main.m_logLoadingError.Write(
        "Quest.dat Index(%d) %dth kind(%d)%s\t\t%d\t\t%d m_RewardMastery[%d].m_nConsMasteryID(%d) > 0 or -1 Invalid",
        iInx,
        iNth,
        pData->m_nConsMasteryID,
        kindNames[pData->m_nConsMasteryID],
        pData->m_nConsMasterySubID,
        pData->m_nConsMasteryCnt,
        iNth,
        pData->m_nConsMasterySubID);
      return false;
    case 3:
      if (pData->m_nConsMasterySubID <= 0 && pData->m_nConsMasterySubID != -1)
      {
        return true;
      }
      g_Main.m_logLoadingError.Write(
        "Quest.dat Index(%d) %dth kind(%d)%s\t\t%d\t\t%d m_RewardMastery[%d].m_nConsMasteryID(%d) > 0 or -1 Invalid",
        iInx,
        iNth,
        pData->m_nConsMasteryID,
        kindNames[pData->m_nConsMasteryID],
        pData->m_nConsMasterySubID,
        pData->m_nConsMasteryCnt,
        iNth,
        pData->m_nConsMasterySubID);
      return false;
    case 4:
      if (pData->m_nConsMasterySubID < 48 && pData->m_nConsMasterySubID != -1)
      {
        return true;
      }
      g_Main.m_logLoadingError.Write(
        "Quest.dat Index(%d) %dth kind(%d)%s\t\t%d\t\t%d m_RewardMastery[%d].m_nConsMasteryID(%d) >= max_skill_num(%d) or -1 Invalid",
        iInx,
        iNth,
        pData->m_nConsMasteryID,
        kindNames[pData->m_nConsMasteryID],
        pData->m_nConsMasterySubID,
        pData->m_nConsMasteryCnt,
        iNth,
        pData->m_nConsMasterySubID,
        48);
      return false;
    case 5:
      if (pData->m_nConsMasterySubID < 24 && pData->m_nConsMasterySubID != -1)
      {
        return true;
      }
      g_Main.m_logLoadingError.Write(
        "Quest.dat Index(%d) %dth kind(%d)%s\t\t%d\t\t%d m_RewardMastery[%d].m_nConsMasteryID(%d) >= force_mastery_num(%d) or -1 Invalid",
        iInx,
        iNth,
        pData->m_nConsMasteryID,
        kindNames[pData->m_nConsMasteryID],
        pData->m_nConsMasterySubID,
        pData->m_nConsMasteryCnt,
        iNth,
        pData->m_nConsMasterySubID,
        24);
      return false;
    case 6:
      if (pData->m_nConsMasterySubID < 3 && pData->m_nConsMasterySubID != -1)
      {
        return true;
      }
      g_Main.m_logLoadingError.Write(
        "Quest.dat Index(%d) %dth kind(%d)%s\t\t%d\t\t%d m_RewardMastery[%d].m_nConsMasteryID(%d) >= MI_CLASS_NUM(%d) or -1 Invalid",
        iInx,
        iNth,
        pData->m_nConsMasteryID,
        kindNames[pData->m_nConsMasteryID],
        pData->m_nConsMasterySubID,
        pData->m_nConsMasteryCnt,
        iNth,
        pData->m_nConsMasterySubID,
        3);
      return false;
    case 7:
      if (pData->m_nConsMasterySubID >= 3 || pData->m_nConsMasterySubID == -1)
      {
        g_Main.m_logLoadingError.Write(
          "Quest.dat Index(%d) %dth kind(%d)%s\t\t%d\t\t%d m_RewardMastery[%d].m_nConsMasteryID(%d) >= MASTERY_CODE_NUM(%d) or -1 Invalid",
          iInx,
          iNth,
          pData->m_nConsMasteryID,
          kindNames[pData->m_nConsMasteryID],
          pData->m_nConsMasterySubID,
          pData->m_nConsMasteryCnt,
          iNth,
          pData->m_nConsMasterySubID,
          3);
        return false;
      }
      return true;
    default:
      g_Main.m_logLoadingError.Write(
        "Quest.dat Index(%d) %dth kind(%d)\t\t%d\t\t%d pData->m_nConsMasteryID(%d) >= MASTERY_CODE_NUM(%d) Invalid",
        iInx,
        iNth,
        pData->m_nConsMasteryID,
        pData->m_nConsMasterySubID,
        pData->m_nConsMasteryCnt,
        pData->m_nConsMasteryID,
        7);
      return false;
  }
}

bool CQuestMgr::CalcStartNPCQuestCnt(unsigned int *pdwCnt)
{
  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(CQuestMgr::s_tblQuestHappenEvent[1].GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }

    for (int j = 0; j < 3; ++j)
    {
      auto *record = reinterpret_cast<_QuestHappenEvent_fld *>(
        CQuestMgr::s_tblQuestHappenEvent[1].GetRecord(n));
      if (std::strcmp(record->m_Node[j].m_strLinkQuest[0], "-1") != 0
        && std::strcmp(record->m_Node[j].m_strLinkQuest[0], "0") != 0
        && record->m_Node[j].m_nQuestType == 1)
      {
        ++pdwCnt[j];
      }
    }
  }

  return true;
}

_quest_check_result *CQuestMgr::CheckReqAct(
  int nActCode,
  char *pszReqCode,
  unsigned __int16 wActCount,
  bool bPartyState)
{
  s_QuestCKRet.init();
  int checkCount = 0;

  for (int slotIndex = 0; slotIndex < 30; ++slotIndex)
  {
    _QUEST_DB_BASE::_LIST *slot = &m_pQuestData->m_List[slotIndex];
    if (slot->byQuestType == 255)
    {
      continue;
    }

    _Quest_fld *record = reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(slot->wIndex));
    if (!record)
    {
      continue;
    }

    if (bPartyState && record->m_nQuestType != 1)
    {
      continue;
    }

    for (int actIndex = 0; actIndex < 3; ++actIndex)
    {
      if (slot->wNum[actIndex] == 65535)
      {
        continue;
      }

      _action_node *action = &record->m_ActionNode[actIndex];
      if (action->m_nActType == -1)
      {
        break;
      }

      if (action->m_nActType != nActCode)
      {
        continue;
      }

      if (nActCode == 6)
      {
        const int reqValue = std::atoi(pszReqCode);
        const int actionValue = std::atoi(action->m_strActSub);
        if (reqValue < actionValue)
        {
          continue;
        }
      }
      else if (nActCode == 14)
      {
        if (std::strncmp(action->m_strActSub, pszReqCode, 5) != 0
          || !__CheckCond_Have(action->m_nReqAct, action->m_strActSub2))
        {
          continue;
        }

        wActCount = static_cast<unsigned __int16>(action->m_nReqAct);
      }
      else if (std::strcmp(action->m_strActSub, pszReqCode))
      {
        continue;
      }

      if (!std::strncmp(action->m_strActArea, "-1", 2) || g_MapOper.IsInRegion(action->m_strActArea, m_pMaster))
      {
        bool allowByOrder = true;
        if (action->m_nOrder > 0)
        {
          allowByOrder = true;
          for (int m = 0; m < 3; ++m)
          {
            if (record->m_ActionNode[m].m_nOrder < action->m_nOrder && slot->wNum[m] != 65535)
            {
              allowByOrder = false;
              break;
            }
          }
        }

        if (allowByOrder && (rand() % 100) < action->m_nSetCntPro_100)
        {
          _quest_check_result::_node *node = &s_QuestCKRet.m_List[checkCount];
          node->byQuestDBSlot = static_cast<unsigned __int8>(slotIndex);
          node->byActIndex = static_cast<unsigned __int8>(actIndex);
          node->wCount = static_cast<unsigned __int16>(slot->wNum[actIndex] + 1);
          node->bORComplete = false;

          if (wActCount + slot->wNum[actIndex] >= action->m_nReqAct)
          {
            node->wCount = 65535;

            if (nActCode == 14 && !DeleteQuestItem(action->m_strActSub2, static_cast<unsigned __int16>(action->m_nReqAct)))
            {
              const char *charName = m_pMaster->m_Param.GetCharNameA();
              g_Main.m_logSystemError.Write(
                "CQuestMgr::CheckReqAct : DeleteQuestItem(%s, %d) : %s",
                action->m_strActSub2,
                action->m_nReqAct,
                charName);
            }

            if (record->m_bCompQuestType)
            {
              node->bORComplete = true;
            }
          }

          ++checkCount;
        }

        break;
      }
    }
  }

  if (checkCount <= 0)
  {
    return nullptr;
  }

  s_QuestCKRet.m_byCheckNum = static_cast<unsigned __int8>(checkCount);
  return &s_QuestCKRet;
}

bool CQuestMgr::CheckFailCondition(unsigned __int8 byQuestDBSlot, int nFailCond, char *pszCode)
{
  char failed = 0;
  if (!m_pMaster)
  {
    return (failed) != 0;
  }

  _QUEST_DB_BASE::_LIST *slot = &m_pMaster->m_Param.m_QuestDB.m_List[byQuestDBSlot];
  _QUEST_DB_BASE::_LIST backup{};
  std::memcpy(&backup, slot, sizeof(backup));

  if (slot->wIndex == 65535)
  {
    return (failed) != 0;
  }

  _Quest_fld *record = reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(slot->wIndex));
  if (!record)
  {
    return (failed) != 0;
  }

  if (slot->dwPassSec == static_cast<unsigned int>(-1))
  {
    return (failed) != 0;
  }

  if (!record->m_bFailCheck)
  {
    return (failed) != 0;
  }

  for (int j = 0; j < 3; ++j)
  {
    if (record->m_QuestFailCond[j].m_nFailCondition != nFailCond)
    {
      continue;
    }

    if (nFailCond)
    {
      if (nFailCond == 1)
      {
        const int limitMin = std::atoi(record->m_QuestFailCond[j].m_strFailCode);
        if (slot->dwPassSec < static_cast<unsigned int>(60 * limitMin))
        {
          failed = 1;
        }
      }
      else if (!pszCode || !std::strcmp(pszCode, record->m_QuestFailCond[j].m_strFailCode))
      {
        failed = 1;
      }
    }
    else
    {
      const int limitMin = std::atoi(record->m_QuestFailCond[j].m_strFailCode);
      if (slot->dwPassSec > static_cast<unsigned int>(60 * limitMin))
      {
        failed = 1;
      }
    }

    if (!failed)
    {
      continue;
    }

    m_pMaster->SendMsg_QuestFailure(static_cast<char>(nFailCond), static_cast<char>(byQuestDBSlot));
    DeleteQuestData(byQuestDBSlot);
    m_pMaster->m_pUserDB->Update_QuestDelete(byQuestDBSlot);

    if (!record->m_nLinkDummyCond && std::strcmp(record->m_strLinkDummyCode, "-1"))
    {
      _dummy_position *dummy = m_pMaster->m_pCurMap->GetDummyPostion(record->m_strLinkDummyCode);
      if (dummy)
      {
        float newPos[3]{};
        if (m_pMaster->m_pCurMap->GetRandPosInDummy(dummy, newPos, 1))
        {
          m_pMaster->OutOfMap(m_pMaster->m_pCurMap, m_pMaster->m_wMapLayerIndex, 3u, newPos);
          const unsigned __int8 mapCode = m_pMaster->m_Param.GetMapCode();
          m_pMaster->SendMsg_GotoRecallResult(0, mapCode, newPos, 4u);
        }
      }
    }

    if (!std::strcmp(record->m_strFailLinkQuest, "-1"))
    {
      return (failed) != 0;
    }

    _Quest_fld *nextRecord = reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(record->m_strFailLinkQuest));
    if (!nextRecord)
    {
      return (failed) != 0;
    }

    if (backup.byQuestType != 1
      || (!IsCompleteNpcQuest(nextRecord->m_strCode, nextRecord->m_bQuestRepeat)
        && !IsProcNpcQuest(nextRecord->m_strCode)))
    {
      slot->byQuestType = backup.byQuestType;
      slot->wIndex = static_cast<unsigned __int16>(nextRecord->m_dwIndex);
      slot->dwPassSec = 0;
      for (int k = 0; k < 3; ++k)
      {
        slot->wNum[k] = 65535;
        if (nextRecord->m_ActionNode[k].m_nActType != -1)
        {
          slot->wNum[k] = 0;
        }
        if (nextRecord->m_ActionNode[k].m_nReqAct == -1)
        {
          slot->wNum[k] = 65535;
        }
      }

      m_pMaster->m_pUserDB->Update_QuestInsert(byQuestDBSlot, slot);
      m_pMaster->SendMsg_InsertNextQuest(byQuestDBSlot, slot);
      return (failed) != 0;
    }
  }

  return (failed) != 0;
}

bool CQuestMgr::DeleteQuestItem(char *pszItemCode, unsigned __int16 wCount)
{
  const int itemTableCode = GetItemTableCode(pszItemCode);
  if (itemTableCode == 255)
  {
    return false;
  }

  _base_fld *record = g_Main.m_tblItemData[itemTableCode].GetRecordByHash(pszItemCode, 2, 5);
  if (!record)
  {
    return false;
  }

  int leftCount = wCount;
  for (int index = 0; ; ++index)
  {
    const unsigned __int8 bagNum = m_pMaster->m_Param.GetBagNum();
    if (index >= 20 * bagNum)
    {
      break;
    }

    _STORAGE_LIST::_db_con *item = &m_pMaster->m_Param.m_dbInven.m_pStorageList[index];
    if (item->m_bLoad
      && item->m_byTableCode == itemTableCode
      && item->m_wItemIndex == static_cast<unsigned __int16>(record->m_dwIndex)
      && !item->m_bLock)
    {
      if (IsOverLapItem(item->m_byTableCode))
      {
        const unsigned __int64 consume = item->m_dwDur > leftCount ? leftCount : item->m_dwDur;
        const unsigned __int64 newDur =
          m_pMaster->Emb_AlterDurPoint(0, index, -static_cast<int>(consume), true, true);
        if (newDur)
        {
          m_pMaster->SendMsg_AdjustAmountInform(0, item->m_wSerial, static_cast<unsigned int>(newDur));
        }
        else
        {
          CPlayer::s_MgrItemHistory.delete_npc_quest_item(
            m_pMaster->m_ObjID.m_wIndex,
            item,
            m_pMaster->m_szItemHistoryFileName);
        }
        leftCount -= static_cast<int>(consume);
      }
      else
      {
        m_pMaster->Emb_DelStorage(0, index, false, true, "CQuestMgr::GiveItem() -- 1");
        CPlayer::s_MgrItemHistory.delete_npc_quest_item(
          m_pMaster->m_ObjID.m_wIndex,
          item,
          m_pMaster->m_szItemHistoryFileName);
        --leftCount;
      }

      if (leftCount <= 0)
      {
        return true;
      }
    }
  }

  return false;
}

bool CQuestMgr::__CheckCond_Have(int nAmonut, char *pszItemCode)
{
  const int itemTableCode = GetItemTableCode(pszItemCode);
  if (itemTableCode == -1)
  {
    return false;
  }

  _base_fld *record = g_Main.m_tblItemData[itemTableCode].GetRecordByHash(pszItemCode, 2, 5);
  if (!record)
  {
    return false;
  }

  int count = 0;
  for (int index = 0; ; ++index)
  {
    const unsigned __int8 bagNum = m_pMaster->m_Param.GetBagNum();
    if (index >= 20 * bagNum)
    {
      break;
    }

    _STORAGE_LIST::_db_con *item = &m_pMaster->m_Param.m_dbInven.m_pStorageList[index];
    if (item->m_bLoad
      && item->m_byTableCode == itemTableCode
      && item->m_wItemIndex == static_cast<unsigned __int16>(record->m_dwIndex))
    {
      if (IsOverLapItem(item->m_byTableCode))
      {
        count += static_cast<int>(item->m_dwDur);
      }
      else
      {
        ++count;
      }

      if (count >= nAmonut)
      {
        return true;
      }
    }
  }

  return false;
}

void CQuestMgr::DeleteQuestData(unsigned __int8 bySlot)
{
  _QUEST_DB_BASE::_LIST *slot = &m_pQuestData->m_List[bySlot];
  if (slot->byQuestType != 255)
  {
    slot->Init();
  }
}

bool CQuestMgr::CanGiveupQuest(unsigned __int8 byQuestDBSlot)
{
  _QUEST_DB_BASE::_LIST *slot = &m_pQuestData->m_List[byQuestDBSlot];
  return slot->byQuestType != 0xFF && slot->byQuestType == 1;
}

bool CQuestMgr::IsCompleteNpcQuest(char *pszCode, int bQuestRepeat)
{
  for (int j = 0; j < 70; ++j)
  {
    _QUEST_DB_BASE::_NPC_QUEST_HISTORY &history = m_pQuestData->m_History[j];
    if (history.byLevel != 255
      && std::strncmp(history.szQuestCode, pszCode, 7) == 0
      && bQuestRepeat != 1)
    {
      return true;
    }
  }
  return false;
}

bool CQuestMgr::IsProcNpcQuest(char *pszCode)
{
  for (int j = 0; j < 30; ++j)
  {
    _QUEST_DB_BASE::_LIST *slot = &m_pQuestData->m_List[j];
    if (slot->byQuestType == 1)
    {
      _base_fld *record = CQuestMgr::s_tblQuest->GetRecord(slot->wIndex);
      if (record && std::strncmp(record->m_strCode, pszCode, 7) == 0)
      {
        return true;
      }
    }
  }
  return false;
}

int CQuestMgr::GetCountQuestType(int nType)
{
  unsigned int count = 0;
  for (int j = 0; j < 30; ++j)
  {
    _QUEST_DB_BASE::_LIST *slot = &m_pQuestData->m_List[j];
    if (slot->byQuestType != 0xFF && slot->byQuestType == nType)
    {
      ++count;
    }
  }
  // narrowing cast for thunk return parity
  return static_cast<int>(count);
}

_Quest_fld *CQuestMgr::GetQuestFromEvent(unsigned __int8 bySelect)
{
  if (!m_LastHappenEvent.isset())
  {
    return nullptr;
  }

  char *recordCode = nullptr;
  int linkCount = 0;
  for (int j = 0; j < 5 && std::strncmp(m_LastHappenEvent.m_pEvent->m_strLinkQuest[j], "-1", 2) != 0; ++j)
  {
    ++linkCount;
  }

  if (linkCount > 0)
  {
    if (m_LastHappenEvent.m_pEvent->m_bSelectQuestManual)
    {
      if (bySelect >= linkCount)
      {
        return nullptr;
      }
      recordCode = m_LastHappenEvent.m_pEvent->m_strLinkQuest[bySelect];
    }
    else
    {
      recordCode = m_LastHappenEvent.m_pEvent->m_strLinkQuest[rand() % linkCount];
    }
  }

  m_LastHappenEvent.init();
  if (recordCode)
  {
    return reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(recordCode));
  }
  return nullptr;
}

unsigned __int8 CQuestMgr::InsertNpcQuestHistory(char *pszQuestCode, char byLevel, long double dRepeatTime)
{
  int bestIndex = 0;
  unsigned __int8 minLevel = 255;

  for (int j = 0; j < 70; ++j)
  {
    _QUEST_DB_BASE::_NPC_QUEST_HISTORY &history = m_pQuestData->m_History[j];
    if (history.byLevel == 255)
    {
      std::strcpy(history.szQuestCode, pszQuestCode);
      history.byLevel = byLevel;
      history.dwEventEndTime = GetConnectTime_AddBySec(static_cast<int>(dRepeatTime));
      return static_cast<unsigned __int8>(j);
    }
    if (history.byLevel < minLevel)
    {
      minLevel = history.byLevel;
      bestIndex = j;
    }
  }

  _QUEST_DB_BASE::_NPC_QUEST_HISTORY &history = m_pQuestData->m_History[bestIndex];
  std::strcpy(history.szQuestCode, pszQuestCode);
  history.byLevel = byLevel;
  history.dwEventEndTime = GetConnectTime_AddBySec(static_cast<int>(dRepeatTime));
  return static_cast<unsigned __int8>(bestIndex);
}
_happen_event_cont *CQuestMgr::CheckQuestHappenEvent(
  QUEST_HAPPEN HappenType,
  char *pszEventCode,
  unsigned __int8 byRaceCode)
{
  _QuestHappenEvent_fld *record = CQuestMgr::s_tblQuestHappenEvent
    ? static_cast<_QuestHappenEvent_fld *>(CQuestMgr::s_tblQuestHappenEvent[HappenType].GetRecord(pszEventCode))
    : nullptr;
  while (record)
  {
    auto *eventNode = &record->m_Node[byRaceCode];
    if (!eventNode->m_bUse)
    {
      break;
    }
    if (eventNode->m_nQuestType && GetCountQuestType(eventNode->m_nQuestType) >= 5)
    {
      return nullptr;
    }

    bool matched = true;
    for (int j = 0; j < 5; ++j)
    {
      _happen_event_condition_node *cond = &eventNode->m_CondNode[j];
      if (cond->m_nCondType == -1)
      {
        break;
      }
      if (!_CheckCondition(cond))
      {
        matched = false;
        break;
      }
    }

    if (matched)
    {
      if (eventNode->m_nAcepProDen != eventNode->m_nAcepProNum
          && rand() % eventNode->m_nAcepProDen >= eventNode->m_nAcepProNum)
      {
        return nullptr;
      }

      if (eventNode->m_nQuestType == 1)
      {
        _Quest_fld *questRecord = CQuestMgr::s_tblQuest
          ? static_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(eventNode->m_strLinkQuest[0]))
          : nullptr;
        if (!questRecord)
        {
          return nullptr;
        }
        if (questRecord->m_nLimLv != -1)
        {
          const int level = static_cast<int>(this->m_pMaster->m_Param.GetLevel());
          if (questRecord->m_nLimLv < level)
          {
            return nullptr;
          }
        }
        if (!IsCompleteNpcQuest(questRecord->m_strCode, questRecord->m_bQuestRepeat)
            && !IsProcNpcQuest(questRecord->m_strCode))
        {
          if (questRecord->m_bQuestRepeat == 1)
          {
            if (!IsProcLinkNpcQuest(questRecord->m_strCode, questRecord->m_nLinkQuestGroupID))
            {
              return nullptr;
            }
            if (!IsPossibleRepeatNpcQuest(questRecord->m_strCode, questRecord->m_nLinkQuestGroupID))
            {
              return nullptr;
            }
          }
        }
        else
        {
          record = CQuestMgr::s_tblQuestHappenEvent
            ? static_cast<_QuestHappenEvent_fld *>(
                CQuestMgr::s_tblQuestHappenEvent[HappenType].GetRecord(record->m_dwIndex + 1))
            : nullptr;
          if (record && std::strcmp(record->m_strCode, pszEventCode))
          {
            record = nullptr;
          }
          continue;
        }
      }

      this->m_LastHappenEvent.set(eventNode, HappenType, record->m_dwIndex, byRaceCode);
      return &this->m_LastHappenEvent;
    }

    record = CQuestMgr::s_tblQuestHappenEvent
      ? static_cast<_QuestHappenEvent_fld *>(CQuestMgr::s_tblQuestHappenEvent[HappenType].GetRecord(record->m_dwIndex + 1))
      : nullptr;
    if (record && std::strcmp(record->m_strCode, pszEventCode))
    {
      record = nullptr;
    }
  }

  return nullptr;
}

void CQuestMgr::CheckNPCQuestList(
  char *pszEventCode,
  unsigned __int8 byRaceCode,
  _NPCQuestIndexTempData *pQuestIndexData)
{
  if (!pszEventCode || !pQuestIndexData || !CQuestMgr::s_tblQuestHappenEvent || !CQuestMgr::s_tblQuest)
  {
    return;
  }

  _QuestHappenEvent_fld *record = static_cast<_QuestHappenEvent_fld *>(CQuestMgr::s_tblQuestHappenEvent[1].GetRecord(pszEventCode));
  if (!record)
  {
    return;
  }

  int happenIndex = static_cast<int>(record->m_dwIndex);
  for (int slot = 0; slot < 30; ++slot)
  {
    record = static_cast<_QuestHappenEvent_fld *>(CQuestMgr::s_tblQuestHappenEvent[1].GetRecord(happenIndex));
    if (!record || std::strcmp(record->m_strCode, pszEventCode) != 0)
    {
      return;
    }

    auto *eventNode = &record->m_Node[byRaceCode];
    if (!eventNode->m_bUse)
    {
      ++happenIndex;
      continue;
    }

    bool conditionPassed = true;
    for (int condIndex = 0; condIndex < 5; ++condIndex)
    {
      _happen_event_condition_node *condition = &eventNode->m_CondNode[condIndex];
      if (condition->m_nCondType == -1)
      {
        break;
      }
      if (!_CheckCondition(condition))
      {
        conditionPassed = false;
        break;
      }
    }
    if (!conditionPassed)
    {
      ++happenIndex;
      continue;
    }

    _Quest_fld *questRecord = static_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(eventNode->m_strLinkQuest[0]));
    if (!questRecord)
    {
      ++happenIndex;
      continue;
    }

    if (questRecord->m_nLimLv != -1)
    {
      const int level = static_cast<int>(m_pMaster->m_Param.GetLevel());
      if (questRecord->m_nLimLv < level)
      {
        ++happenIndex;
        continue;
      }
    }

    const int repeatType = questRecord->m_bQuestRepeat;
    if (!IsCompleteNpcQuest(questRecord->m_strCode, repeatType) && !IsProcNpcQuest(questRecord->m_strCode))
    {
      if (repeatType == 1)
      {
        const int linkGroup = questRecord->m_nLinkQuestGroupID;
        if (!IsProcLinkNpcQuest(questRecord->m_strCode, linkGroup))
        {
          ++happenIndex;
          continue;
        }
        if (!IsPossibleRepeatNpcQuest(questRecord->m_strCode, linkGroup))
        {
          ++happenIndex;
          continue;
        }
      }

      pQuestIndexData->IndexData[pQuestIndexData->nQuestNum].dwQuestIndex = questRecord->m_dwIndex;
      pQuestIndexData->IndexData[pQuestIndexData->nQuestNum].dwQuestHappenIndex =
        static_cast<unsigned int>(happenIndex);
      ++pQuestIndexData->nQuestNum;
    }

    ++happenIndex;
  }
}

_happen_event_cont *CQuestMgr::CheckNPCQuestStartable(
  char *pszEventCode,
  unsigned __int8 byRaceCode,
  int dwQuestIndex,
  unsigned int dwHappenIndex)
{
  if (!pszEventCode || !CQuestMgr::s_tblQuestHappenEvent || !CQuestMgr::s_tblQuest)
  {
    return nullptr;
  }

  _QuestHappenEvent_fld *eventRecord =
    static_cast<_QuestHappenEvent_fld *>(CQuestMgr::s_tblQuestHappenEvent[1].GetRecord(dwHappenIndex));
  if (!eventRecord || std::strcmp(pszEventCode, eventRecord->m_strCode) != 0)
  {
    return nullptr;
  }

  _Quest_fld *questRecord = static_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(dwQuestIndex));
  if (!questRecord)
  {
    return nullptr;
  }

  auto *eventNode = &eventRecord->m_Node[byRaceCode];
  if (!eventNode->m_bUse)
  {
    return nullptr;
  }

  _Quest_fld *linkedQuest = static_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(eventNode->m_strLinkQuest[0]));
  if (!linkedQuest || linkedQuest != questRecord)
  {
    return nullptr;
  }

  for (int condIndex = 0; condIndex < 5; ++condIndex)
  {
    _happen_event_condition_node *condition = &eventNode->m_CondNode[condIndex];
    if (condition->m_nCondType == -1)
    {
      break;
    }
    if (!_CheckCondition(condition))
    {
      return nullptr;
    }
  }

  m_LastHappenEvent.set(
    eventNode,
    quest_happen_type_npc,
    static_cast<int>(eventRecord->m_dwIndex),
    static_cast<int>(byRaceCode));
  return &m_LastHappenEvent;
}


_quest_fail_result *CQuestMgr::CheckLimLv(int nNewLv)
{
  s_QuestFTRet.init();
  int count = 0;

  for (int j = 0; j < 30; ++j)
  {
    _QUEST_DB_BASE::_LIST *slot = &this->m_pQuestData->m_List[j];
    if (slot->wIndex != 65535)
    {
        _Quest_fld *record = CQuestMgr::s_tblQuest
          ? static_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(slot->wIndex))
          : nullptr;
        if (record)
        {
          if (record->m_nLimLv != -1
            && record->m_nLimLv < nNewLv)
          {
            s_QuestFTRet.m_List[count++].byQuestDBSlot = static_cast<unsigned __int8>(j);
          }
      }
    }
  }

  if (count <= 0)
  {
    return nullptr;
  }

  s_QuestFTRet.m_byCheckNum = static_cast<unsigned __int8>(count);
  return &s_QuestFTRet;
}


bool CQuestMgr::IsProcLinkNpcQuest(char *pszCode, int nLinkQuestGroupID)
{
for (int j = 0; j < 30; ++j)
  {
    _QUEST_DB_BASE::_LIST *slot = &this->m_pQuestData->m_List[j];
    if (slot->byQuestType == 1)
    {
      _Quest_fld *record = CQuestMgr::s_tblQuest
        ? static_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(slot->wIndex))
        : nullptr;
      if (record)
      {
        if (record->m_nLinkQuestGroupID == nLinkQuestGroupID)
        {
          return false;
        }
      }
    }
  }
  return true;
}


bool CQuestMgr::IsPossibleRepeatNpcQuest(char *pszCode, int nLinkQuestGroupID)
{
int repeatCount = 0;
  for (int j = 0; j < 70; ++j)
  {
    _QUEST_DB_BASE::_NPC_QUEST_HISTORY &history = this->m_pQuestData->m_History[j];
    if (history.byLevel != 255)
    {
      _Quest_fld *record =
        CQuestMgr::s_tblQuest ? static_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(history.szQuestCode)) : nullptr;
      if (record && record->m_nLinkQuestGroupID == nLinkQuestGroupID)
      {
        const unsigned long long now = GetConnectTime_AddBySec(0);
        if (history.dwEventEndTime >= now)
        {
          ++repeatCount;
        }
      }
    }
  }
  return repeatCount == 0;
}


bool CQuestMgr::_CheckCondition(_happen_event_condition_node *pCond)
{
  switch (pCond->m_nCondType)
  {
    case 0:
      return __CheckCond_LV(pCond->m_nCondSubType, atoi(pCond->m_sCondVal)) != 0;
    case 1:
      return __CheckCond_Grade(pCond->m_nCondSubType, atoi(pCond->m_sCondVal)) != 0;
    case 2:
      return __CheckCond_Dalant(pCond->m_nCondSubType, atoi(pCond->m_sCondVal)) != 0;
    case 3:
      return __CheckCond_Gold(pCond->m_nCondSubType, atoi(pCond->m_sCondVal)) != 0;
    case 4:
      return __CheckCond_Party(pCond->m_nCondSubType);
    case 5:
      return __CheckCond_Guild(pCond->m_nCondSubType);
    case 6:
      return __CheckCond_Nation(pCond->m_nCondSubType);
    case 7:
      return __CheckCond_Equip(pCond->m_sCondVal);
    case 8:
      return __CheckCond_Have(pCond->m_nCondSubType, pCond->m_sCondVal) != 0;
    case 9:
      return __CheckCond_Mastery(pCond->m_nCondSubType, pCond->m_sCondVal);
    case 10:
      return __CheckCond_Dummy(pCond->m_nCondSubType, pCond->m_sCondVal) != 0;
    case 11:
      return __CheckCond_Race(pCond->m_sCondVal);
    case 12:
      return __CheckCond_Class(pCond->m_sCondVal) != 0;
    default:
      return false;
  }
}


bool CQuestMgr::__CheckCond_Class(char *pszClassCode)
{
  for (int j = 0; j < 4; ++j)
  {
    _class_fld *effectRecord = *m_pMaster->m_Param.m_ppHistoryEffect[j];
    if (!effectRecord)
    {
      break;
    }
    if (!std::strcmp(effectRecord->m_strCode, pszClassCode))
    {
      return true;
    }
  }
  return false;
}


bool CQuestMgr::__CheckCond_Dummy(int bInclude, char *pszDummyCode)
{
return true;// this is not a stub
}


bool CQuestMgr::__CheckCond_LV(unsigned __int8 byCompare, int nLv)
{
  if (byCompare)
  {
    if (byCompare == 1)
    {
    if (static_cast<unsigned int>(this->m_pMaster->m_Param.GetLevel()) < static_cast<unsigned int>(nLv))
      {
        return true;
      }
    }
    else if (byCompare == 2
             && static_cast<unsigned int>(this->m_pMaster->m_Param.GetLevel()) > static_cast<unsigned int>(nLv))
    {
      return true;
    }
  }
  else if (static_cast<unsigned int>(this->m_pMaster->m_Param.GetLevel()) == static_cast<unsigned int>(nLv))
  {
    return true;
  }
  return false;
}


bool CQuestMgr::__CheckCond_Guild(int belong)
{
  if (belong)
  {
    return this->m_pMaster->m_Param.m_pGuild == nullptr;
  }
  return this->m_pMaster->m_Param.m_pGuild != nullptr;
}


bool CQuestMgr::__CheckCond_Nation(int belong)
{
return false; // this is not a stub
}


bool CQuestMgr::__CheckCond_Race(char *pszRaceSexFlag)
{
  const int raceSex = static_cast<int>(this->m_pMaster->m_Param.GetRaceSexCode());
  return pszRaceSexFlag[raceSex] == '1';
}


bool CQuestMgr::__CheckCond_Party(int belong)
{
  if (belong)
  {
    return this->m_pMaster->m_pPartyMgr && !this->m_pMaster->m_pPartyMgr->IsPartyMode();
  }
  return this->m_pMaster->m_pPartyMgr && this->m_pMaster->m_pPartyMgr->IsPartyMode();
}


bool CQuestMgr::__CheckCond_Mastery(unsigned __int8 byCompare, char *pszMasteryPack)
{
char codeBuf[32]{};
  char indexBuf[32]{};
  char levelBuf[16]{};

  std::memcpy(codeBuf, pszMasteryPack, 1u);
  codeBuf[1] = 0;
  std::memcpy(indexBuf, pszMasteryPack + 1, 2u);
  indexBuf[2] = 0;
  std::memcpy(levelBuf, pszMasteryPack + 3, 2u);
  levelBuf[2] = 0;

  const int code = atoi(codeBuf);
  const int index = atoi(indexBuf);
  const int level = atoi(levelBuf);
  if (!_MASTERY_PARAM::IsValidMasteryCode(code, index))
  {
    return false;
  }
  const int mastery = this->m_pMaster->m_pmMst.GetMasteryPerMast(
    static_cast<unsigned __int8>(code),
    static_cast<unsigned __int8>(index));
  return mastery >= level;
}


bool CQuestMgr::__CheckCond_Dalant(unsigned __int8 byCompare, unsigned int nDalant)
{
  if (byCompare)
  {
    if (byCompare == 1)
    {
      if (this->m_pMaster->m_Param.GetDalant() < nDalant)
      {
        return true;
      }
    }
    else if (byCompare == 2 && this->m_pMaster->m_Param.GetDalant() > nDalant)
    {
      return true;
    }
  }
  else if (this->m_pMaster->m_Param.GetDalant() == nDalant)
  {
    return true;
  }
  return false;
}


bool CQuestMgr::__CheckCond_Gold(unsigned __int8 byCompare, unsigned int nGold)
{
  if (byCompare)
  {
    if (byCompare == 1)
    {
      if (this->m_pMaster->m_Param.GetGold() < nGold)
      {
        return true;
      }
    }
    else if (byCompare == 2 && this->m_pMaster->m_Param.GetGold() > nGold)
    {
      return true;
    }
  }
  else if (this->m_pMaster->m_Param.GetGold() == nGold)
  {
    return true;
  }
  return false;
}


bool CQuestMgr::__CheckCond_Grade(unsigned __int8 byCompare, int nGrade)
{
  if (byCompare)
  {
    if (byCompare == 1)
    {
      if (this->m_pMaster->m_Param.m_byPvPGrade < nGrade)
      {
        return true;
      }
    }
    else if (byCompare == 2 && this->m_pMaster->m_Param.m_byPvPGrade > nGrade)
    {
      return true;
    }
  }
  else if (this->m_pMaster->m_Param.m_byPvPGrade == nGrade)
  {
    return true;
  }
  return false;
}


bool CQuestMgr::__CheckCond_Equip(char *pszItemCode)
{
  const int tableCode = GetItemTableCode(pszItemCode);
  if (tableCode == -1)
  {
    return false;
  }
  _STORAGE_LIST::_db_con *equip = &this->m_pMaster->m_Param.m_dbEquip.m_pStorageList[tableCode];
  if (!equip->m_bLoad)
  {
    return false;
  }
  _base_fld *record = g_Main.m_tblItemData[tableCode].GetRecord(equip->m_wItemIndex);
  return record && strncmp(record->m_strCode, pszItemCode, 7u) == 0;
}
