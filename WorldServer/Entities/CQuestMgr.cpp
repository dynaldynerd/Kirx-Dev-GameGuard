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

#include <cstring>

CRecordData *CQuestMgr::s_tblQuest = nullptr;
CRecordData *CQuestMgr::s_tblQuestHappenEvent = nullptr;

namespace
{
  _quest_check_result s_QuestCKRet{};
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
        if (!CQuestMgr::s_tblQuestHappenEvent[j].ReadRecord(szFile[j], 0x888u, pszErrMsg))
        {
            MyMessageBox("CQuestMgr Data Load", "%s Load Error", szFile[j]);
            ServerProgramExit("CQuestMgr Data Load", 0);
        }
    }

    if (!CQuestMgr::s_tblQuest)
    {
        CQuestMgr::s_tblQuest = new CRecordData();
    }
    if(!CQuestMgr::s_tblQuest->ReadRecord(".\\script\\Quest.dat", 0x968u, pszErrMsg))
    {
        MyMessageBox("CQuestMgr Data Load", "Quest.dat Load Error");
        ServerProgramExit("CQuestMgr Data Load", 0);
  }
    _Quest_fld *Record = 0LL;
    bool v12 = false;
  for (int n = 0; ; ++n)
  {
    int RecordNum = CQuestMgr::s_tblQuest->GetRecordNum();
    if (n >= RecordNum)
      break;
        Record = (_Quest_fld*)CQuestMgr::s_tblQuest->GetRecord(n);
        if (Record)
        {
            if (!CQuestMgr::CheckRewardMasteryData(n, 0, Record->m_RewardMastery))
                v12 = true;
            if (!CQuestMgr::CheckRewardMasteryData(n, 1, &Record->m_RewardMastery[1]))
                v12 = true;
        }
    else
    {
      MyMessageBox("Error", "CQuestMgr::LoadQuestData() : s_tblQuest[%d] NULL!", n);
    }
  }
  if (!v12)
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
    if (slot->byQuestType == 0xFF)
    {
      continue;
    }

    _base_fld *record = CRecordData::GetRecord(CQuestMgr::s_tblQuest, slot->wIndex);
    if (!record)
    {
      continue;
    }

    if (bPartyState && *reinterpret_cast<int *>(record[1].m_strCode) != 1)
    {
      continue;
    }

    for (int actIndex = 0; actIndex < 3; ++actIndex)
    {
      if (slot->wNum[actIndex] == 0xFFFF)
      {
        continue;
      }

      char *actBase = &record[4 * actIndex + 1].m_strCode[32];
      if (*reinterpret_cast<int *>(actBase) == -1)
      {
        break;
      }

      if (*reinterpret_cast<int *>(actBase) != nActCode)
      {
        continue;
      }

      if (nActCode == 6)
      {
        const int reqVal = std::atoi(pszReqCode);
        const int actVal = std::atoi(actBase + 4);
        if (reqVal < actVal)
        {
          continue;
        }
      }
      else if (nActCode == 14)
      {
        if (std::strncmp(actBase + 4, pszReqCode, 5) != 0
          || !__CheckCond_Have(*reinterpret_cast<int *>(actBase + 196), actBase + 68))
        {
          continue;
        }
        wActCount = *reinterpret_cast<unsigned __int16 *>(actBase + 196);
      }
      else if (strcmp_0(actBase + 4, pszReqCode) != 0)
      {
        continue;
      }

      if (std::strncmp(actBase + 132, "-1", 2) == 0
        || CMapOperation::IsInRegion(&g_MapOper, actBase + 132, m_pMaster))
      {
        bool allowAct = true;
        const int reqCond = *reinterpret_cast<int *>(actBase + 268);
        if (reqCond > 0)
        {
          allowAct = true;
          for (int m = 0; m < 3; ++m)
          {
            if (*reinterpret_cast<int *>(&record[4 * m + 5].m_strCode[28]) < reqCond
              && slot->wNum[m] != 0xFFFF)
            {
              allowAct = false;
              break;
            }
          }
        }

        if (allowAct && (rand() % 100) < *reinterpret_cast<int *>(actBase + 200))
        {
          _quest_check_result::_node *node = &s_QuestCKRet.m_List[checkCount];
          node->byQuestDBSlot = static_cast<unsigned __int8>(slotIndex);
          node->byActIndex = static_cast<unsigned __int8>(actIndex);
          node->wCount = static_cast<unsigned __int16>(slot->wNum[actIndex] + 1);
          node->bORComplete = false;
          if (wActCount + slot->wNum[actIndex] >= *reinterpret_cast<int *>(actBase + 196))
          {
            node->wCount = 0xFFFF;
            if (nActCode == 14
              && !DeleteQuestItem(actBase + 68, *reinterpret_cast<unsigned __int16 *>(actBase + 196)))
            {
              const char *charName = CPlayerDB::GetCharNameA(&m_pMaster->m_Param);
              CLogFile::Write(
                &g_Main.m_logSystemError,
                "CQuestMgr::CheckReqAct : DeleteQuestItem(%s, %d) : %s",
                actBase + 68,
                *reinterpret_cast<int *>(actBase + 196),
                charName);
            }
            if (*reinterpret_cast<int *>(&record[1].m_strCode[28]))
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

char CQuestMgr::CheckFailCondition(unsigned __int8 byQuestDBSlot, int nFailCond, char *pszCode)
{
  char failed = 0;
  if (!m_pMaster)
  {
    return failed;
  }

  _QUEST_DB_BASE::_LIST *slot = &m_pMaster->m_Param.m_QuestDB.m_List[byQuestDBSlot];
  _QUEST_DB_BASE::_LIST backup{};
  memcpy_0(&backup, slot, sizeof(backup));

  if (*reinterpret_cast<unsigned __int16 *>(reinterpret_cast<char *>(slot) + 1) == 0xFFFF)
  {
    return failed;
  }

  _base_fld *record = CRecordData::GetRecord(CQuestMgr::s_tblQuest, slot->wIndex);
  if (!record)
  {
    return failed;
  }

  if (*reinterpret_cast<int *>(reinterpret_cast<char *>(slot) + 9) == -1)
  {
    return failed;
  }

  if (!*reinterpret_cast<int *>(&record[27].m_strCode[28]))
  {
    return failed;
  }

  for (int j = 0; j < 3; ++j)
  {
    if (*reinterpret_cast<int *>(&record[j + 27].m_strCode[32]) != nFailCond)
    {
      continue;
    }

    if (nFailCond)
    {
      if (nFailCond == 1)
      {
        const int limitMin = std::atoi(&record[j + 27].m_strCode[36]);
        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(slot) + 9) < (60 * limitMin))
        {
          failed = 1;
        }
      }
      else if (!pszCode || !strcmp_0(pszCode, &record[j + 27].m_strCode[36]))
      {
        failed = 1;
      }
    }
    else
    {
      const int limitMin = std::atoi(&record[j + 27].m_strCode[36]);
      if (*reinterpret_cast<int *>(reinterpret_cast<char *>(slot) + 9) > (60 * limitMin))
      {
        failed = 1;
      }
    }

    if (failed)
    {
      m_pMaster->SendMsg_QuestFailure(static_cast<char>(nFailCond), static_cast<char>(byQuestDBSlot));
      DeleteQuestData(byQuestDBSlot);
      CUserDB::Update_QuestDelete(m_pMaster->m_pUserDB, byQuestDBSlot);

      if (!*reinterpret_cast<int *>(&record[31].m_strCode[28]))
      {
        if (strcmp_0(&record[31].m_strCode[32], "-1") != 0)
        {
          _dummy_position *pos = CMapData::GetDummyPostion(m_pMaster->m_pCurMap, &record[31].m_strCode[32]);
          if (pos)
          {
            float newPos[3]{};
            if (CMapData::GetRandPosInDummy(m_pMaster->m_pCurMap, pos, newPos, 1))
            {
              m_pMaster->OutOfMap(m_pMaster->m_pCurMap, m_pMaster->m_wMapLayerIndex, 3, newPos);
              const unsigned __int8 mapCode = CPlayerDB::GetMapCode(&m_pMaster->m_Param);
              m_pMaster->SendMsg_GotoRecallResult(0, mapCode, newPos, 4);
            }
          }
        }
      }

      if (!strcmp_0(&record[32].m_strCode[28], "-1"))
      {
        return failed;
      }

      _base_fld *nextRecord = CRecordData::GetRecord(CQuestMgr::s_tblQuest, &record[32].m_strCode[28]);
      if (!nextRecord)
      {
        return failed;
      }

      if (backup.byQuestType != 1
        || (!IsCompleteNpcQuest(nextRecord->m_strCode, *reinterpret_cast<int *>(&nextRecord[1].m_strCode[4]))
          && !IsProcNpcQuest(nextRecord->m_strCode)))
      {
        slot->byQuestType = backup.byQuestType;
        slot->wIndex = static_cast<unsigned __int16>(nextRecord->m_dwIndex);
        slot->dwPassSec = 0;
        for (int k = 0; k < 3; ++k)
        {
          slot->wNum[k] = 0xFFFF;
          if (*reinterpret_cast<int *>(&nextRecord[4 * k + 1].m_strCode[32]) != -1)
          {
            slot->wNum[k] = 0;
          }
          if (*reinterpret_cast<int *>(&nextRecord[4 * k + 4].m_strCode[24]) == -1)
          {
            slot->wNum[k] = 0xFFFF;
          }
        }

        CUserDB::Update_QuestInsert(m_pMaster->m_pUserDB, byQuestDBSlot, slot);
        m_pMaster->SendMsg_InsertNextQuest(byQuestDBSlot, slot);
        return failed;
      }
    }
  }

  return failed;
}

char CQuestMgr::DeleteQuestItem(char *pszItemCode, unsigned __int16 wCount)
{
  const int itemTableCode = GetItemTableCode(pszItemCode);
  if (itemTableCode == 0xFF)
  {
    return 0;
  }

  _base_fld *record = CRecordData::GetRecordByHash(&g_Main.m_tblItemData[itemTableCode], pszItemCode, 2, 5);
  if (!record)
  {
    return 0;
  }

  int leftCount = wCount;
  for (int index = 0; ; ++index)
  {
    const unsigned __int8 bagNum = CPlayerDB::GetBagNum(&m_pMaster->m_Param);
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
          CPlayer::Emb_AlterDurPoint(m_pMaster, 0, index, -static_cast<int>(consume), true, true);
        if (newDur)
        {
          CPlayer::SendMsg_AdjustAmountInform(m_pMaster, 0, item->m_wSerial, newDur);
        }
        else
        {
          CMgrAvatorItemHistory::delete_npc_quest_item(
            &CPlayer::s_MgrItemHistory,
            m_pMaster->m_ObjID.m_wIndex,
            item,
            m_pMaster->m_szItemHistoryFileName);
        }
        leftCount -= static_cast<int>(consume);
      }
      else
      {
        CPlayer::Emb_DelStorage(m_pMaster, 0, index, false, true, "CQuestMgr::GiveItem() -- 1");
        CMgrAvatorItemHistory::delete_npc_quest_item(
          &CPlayer::s_MgrItemHistory,
          m_pMaster->m_ObjID.m_wIndex,
          item,
          m_pMaster->m_szItemHistoryFileName);
        --leftCount;
      }

      if (leftCount <= 0)
      {
        return 1;
      }
    }
  }

  return 0;
}

char CQuestMgr::__CheckCond_Have(int nAmonut, char *pszItemCode)
{
  const int itemTableCode = GetItemTableCode(pszItemCode);
  if (itemTableCode == -1)
  {
    return 0;
  }

  _base_fld *record = CRecordData::GetRecordByHash(&g_Main.m_tblItemData[itemTableCode], pszItemCode, 2, 5);
  if (!record)
  {
    return 0;
  }

  int count = 0;
  for (int index = 0; ; ++index)
  {
    const unsigned __int8 bagNum = CPlayerDB::GetBagNum(&m_pMaster->m_Param);
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
        return 1;
      }
    }
  }

  return 0;
}

void CQuestMgr::DeleteQuestData(unsigned __int8 bySlot)
{
  _QUEST_DB_BASE::_LIST *slot = &m_pQuestData->m_List[bySlot];
  if (slot->byQuestType != 0xFF)
  {
    slot->Init();
  }
}

char CQuestMgr::IsCompleteNpcQuest(char *pszCode, int bQuestRepeat)
{
  for (int j = 0; j < 70; ++j)
  {
    char *entry = m_pQuestData->m_History[j].szQuestCode;
    if (static_cast<unsigned __int8>(entry[12]) != 0xFF
      && std::strncmp(entry, pszCode, 7) == 0
      && bQuestRepeat != 1)
    {
      return 1;
    }
  }
  return 0;
}

char CQuestMgr::IsProcNpcQuest(char *pszCode)
{
  for (int j = 0; j < 30; ++j)
  {
    _QUEST_DB_BASE::_LIST *slot = &m_pQuestData->m_List[j];
    if (slot->byQuestType == 1)
    {
      _base_fld *record = CRecordData::GetRecord(CQuestMgr::s_tblQuest, slot->wIndex);
      if (record && std::strncmp(record->m_strCode, pszCode, 7) == 0)
      {
        return 1;
      }
    }
  }
  return 0;
}

__int64 CQuestMgr::GetCountQuestType(int nType)
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
  return count;
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
    return reinterpret_cast<_Quest_fld *>(CRecordData::GetRecord(CQuestMgr::s_tblQuest, recordCode));
  }
  return nullptr;
}

unsigned __int8 CQuestMgr::InsertNpcQuestHistory(char *pszQuestCode, char byLevel, long double dRepeatTime)
{
  int bestIndex = 0;
  unsigned __int8 minLevel = 0xFF;

  for (int j = 0; j < 70; ++j)
  {
    char *entry = m_pQuestData->m_History[j].szQuestCode;
    if (static_cast<unsigned __int8>(entry[12]) == 0xFF)
    {
      strcpy_0(entry, pszQuestCode);
      entry[12] = byLevel;
      *reinterpret_cast<unsigned int *>(entry + 13) = GetConnectTime_AddBySec(static_cast<int>(dRepeatTime));
      return static_cast<unsigned __int8>(j);
    }
    if (static_cast<unsigned __int8>(entry[12]) < minLevel)
    {
      minLevel = static_cast<unsigned __int8>(entry[12]);
      bestIndex = j;
    }
  }

  char *entry = m_pQuestData->m_History[bestIndex].szQuestCode;
  strcpy_0(entry, pszQuestCode);
  entry[12] = byLevel;
  *reinterpret_cast<unsigned int *>(entry + 13) = GetConnectTime_AddBySec(static_cast<int>(dRepeatTime));
  return static_cast<unsigned __int8>(bestIndex);
}
