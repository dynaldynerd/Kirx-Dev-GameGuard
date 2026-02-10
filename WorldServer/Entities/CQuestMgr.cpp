#include "pch.h"

#include "CQuestMgr.h"

#include "CPlayer.h"
#include "CRecordData.h"
#include "CMapData.h"
#include "CMapOperation.h"
#include "CLogFile.h"
#include "CUserDB.h"
#include "CPartyPlayer.h"
#include "QuestHappenEvent_fld.h"
#include "WorldServerUtil.h"

#include <cstring>

CRecordData *CQuestMgr::s_tblQuest = nullptr;
CRecordData *CQuestMgr::s_tblQuestHappenEvent = nullptr;
static _quest_check_result s_QuestCKRet{};
static _quest_fail_result s_QuestFTRet{};

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

_quest_check_result *CQuestMgr::CheckReqAct(
  int nActCode,
  char *pszReqCode,
  unsigned __int16 wActCount,
  bool bPartyState)
{
  s_QuestCKRet.init();
  int checkCount = 0;

  for (int questSlot = 0; questSlot < 30; ++questSlot)
  {
    _QUEST_DB_BASE::_LIST *questData = &m_pQuestData->m_List[questSlot];
    if (questData->byQuestType == 255)
    {
      continue;
    }

    _base_fld *record = CQuestMgr::s_tblQuest ? CQuestMgr::s_tblQuest->GetRecord(questData->wIndex) : nullptr;
    if (!record)
    {
      continue;
    }

    if (bPartyState && *reinterpret_cast<unsigned int *>(record[1].m_strCode) != 1)
    {
      continue;
    }

    for (int actIndex = 0; actIndex < 3; ++actIndex)
    {
      if (questData->wNum[actIndex] == 0xFFFF)
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
        const int paramValue = atoi(pszReqCode);
        const int reqValue = atoi(actBase + 4);
        if (paramValue < reqValue)
        {
          continue;
        }
      }
      else if (nActCode == 14)
      {
        if (strncmp(actBase + 4, pszReqCode, 5)
            || !__CheckCond_Have(*reinterpret_cast<int *>(actBase + 196), actBase + 68))
        {
          continue;
        }
        wActCount = *reinterpret_cast<unsigned __int16 *>(actBase + 196);
      }
      else if (strcmp_0(actBase + 4, pszReqCode))
      {
        continue;
      }

      if (strncmp(actBase + 132, "-1", 2) != 0
          && !g_MapOper.IsInRegion(actBase + 132, m_pMaster))
      {
        continue;
      }

      if (*reinterpret_cast<int *>(actBase + 268) > 0)
      {
        bool canPass = true;
        for (int checkIndex = 0; checkIndex < 3; ++checkIndex)
        {
          if (*reinterpret_cast<int *>(&record[4 * checkIndex + 5].m_strCode[28]) <
                *reinterpret_cast<int *>(actBase + 268)
              && questData->wNum[checkIndex] != 0xFFFF)
          {
            canPass = false;
            break;
          }
        }
        if (!canPass)
        {
          break;
        }
      }

      if (rand() % 100 < *reinterpret_cast<int *>(actBase + 200))
      {
        _quest_check_result::_node *node = &s_QuestCKRet.m_List[checkCount];
        node->byQuestDBSlot = static_cast<unsigned __int8>(questSlot);
        node->byActIndex = static_cast<unsigned __int8>(actIndex);
        node->wCount = questData->wNum[actIndex] + 1;
        node->bORComplete = false;

        if (wActCount + questData->wNum[actIndex] >= *reinterpret_cast<int *>(actBase + 196))
        {
          node->wCount = 0xFFFF;
          if (nActCode == 14 && !DeleteQuestItem(actBase + 68, *reinterpret_cast<unsigned __int16 *>(actBase + 196)))
          {
            const char *charNameA = m_pMaster->m_Param.GetCharNameA();
            g_Main.m_logSystemError.Write(
              "CQuestMgr::CheckReqAct : DeleteQuestItem(%s, %d) : %s",
              actBase + 68,
              *reinterpret_cast<int *>(actBase + 196),
              charNameA);
          }
          if (*reinterpret_cast<unsigned int *>(&record[1].m_strCode[28]))
          {
            node->bORComplete = true;
          }
        }

        ++checkCount;
      }
      break;
    }
  }

  if (checkCount <= 0)
  {
    return nullptr;
  }

  s_QuestCKRet.m_byCheckNum = static_cast<unsigned __int8>(checkCount);
  return &s_QuestCKRet;
}

char CQuestMgr::__CheckCond_Have(int nAmount, char *pszItemCode)
{
  const int itemTableCode = GetItemTableCode(pszItemCode);
  if (itemTableCode == -1)
  {
    return 0;
  }

  _base_fld *record =
    g_Main.m_tblItemData[itemTableCode].GetRecordByHash(pszItemCode, 2, 5);
  if (!record)
  {
    return 0;
  }

  int totalCount = 0;
  const unsigned __int8 bagNum = m_pMaster->m_Param.GetBagNum();
  for (int j = 0; j < 20 * bagNum; ++j)
  {
    _STORAGE_LIST::_db_con *item = &m_pMaster->m_Param.m_dbInven.m_pStorageList[j];
    if (item->m_bLoad
        && item->m_byTableCode == itemTableCode
        && item->m_wItemIndex == static_cast<unsigned __int16>(record->m_dwIndex))
    {
      if (IsOverLapItem(item->m_byTableCode))
      {
        totalCount += static_cast<int>(item->m_dwDur);
      }
      else
      {
        ++totalCount;
      }

      if (totalCount >= nAmount)
      {
        return 1;
      }
    }
  }

  return 0;
}

char CQuestMgr::DeleteQuestItem(char *pszItemCode, unsigned __int16 wCount)
{
  const int itemTableCode = GetItemTableCode(pszItemCode);
  if (itemTableCode == 255)
  {
    return 0;
  }

  _base_fld *record =
    g_Main.m_tblItemData[itemTableCode].GetRecordByHash(pszItemCode, 2, 5);
  if (!record)
  {
    return 0;
  }

  int remainCount = wCount;
  const unsigned __int8 bagNum = m_pMaster->m_Param.GetBagNum();
  for (int j = 0; j < 20 * bagNum; ++j)
  {
    _STORAGE_LIST::_db_con *item = &m_pMaster->m_Param.m_dbInven.m_pStorageList[j];
    if (!item->m_bLoad
        || item->m_byTableCode != itemTableCode
        || item->m_wItemIndex != static_cast<unsigned __int16>(record->m_dwIndex)
        || item->m_bLock)
    {
      continue;
    }

    if (IsOverLapItem(item->m_byTableCode))
    {
      const unsigned __int64 takeCount = item->m_dwDur > static_cast<unsigned __int64>(remainCount)
        ? static_cast<unsigned __int64>(remainCount)
        : item->m_dwDur;
      const int takeAmount = static_cast<int>(takeCount);
      const unsigned int dwDur = m_pMaster->Emb_AlterDurPoint(0, j, -takeAmount, 1, 1);
      if (dwDur)
      {
        m_pMaster->SendMsg_AdjustAmountInform(0, item->m_wSerial, dwDur);
      }
      else
      {
        CPlayer::s_MgrItemHistory.delete_npc_quest_item(
          m_pMaster->m_ObjID.m_wIndex,
          item,
          m_pMaster->m_szItemHistoryFileName);
      }
      remainCount -= takeAmount;
    }
    else
    {
      m_pMaster->Emb_DelStorage(0, j, 0, 1, "CQuestMgr::GiveItem() -- 1");
      CPlayer::s_MgrItemHistory.delete_npc_quest_item(
        m_pMaster->m_ObjID.m_wIndex,
        item,
        m_pMaster->m_szItemHistoryFileName);
      --remainCount;
    }

    if (remainCount <= 0)
    {
      return 1;
    }
  }

  return 0;
}

unsigned __int8 CQuestMgr::InsertNpcQuestHistory(char *pszQuestCode, char byLevel, long double dRepeatTime)
{
  unsigned int oldestIndex = 0;
  unsigned __int8 lowestLevel = static_cast<unsigned __int8>(-1);

  for (int j = 0; j < 70; ++j)
  {
    char *dest = this->m_pQuestData->m_History[j].szQuestCode;
    if (static_cast<unsigned __int8>(dest[12]) == 0xFF)
    {
      strcpy_0(dest, pszQuestCode);
      dest[12] = byLevel;
      *reinterpret_cast<unsigned int *>(dest + 13) = GetConnectTime_AddBySec(static_cast<int>(dRepeatTime));
      return static_cast<unsigned __int8>(j);
    }

    if (static_cast<unsigned __int8>(dest[12]) < lowestLevel)
    {
      lowestLevel = static_cast<unsigned __int8>(dest[12]);
      oldestIndex = static_cast<unsigned int>(j);
    }
  }

  char *dest = this->m_pQuestData->m_History[oldestIndex].szQuestCode;
  strcpy_0(dest, pszQuestCode);
  dest[12] = byLevel;
  *reinterpret_cast<unsigned int *>(dest + 13) = GetConnectTime_AddBySec(static_cast<int>(dRepeatTime));
  return static_cast<unsigned __int8>(oldestIndex);
}

_quest_fail_result *CQuestMgr::CheckLimLv(int nNewLv)
{
  s_QuestFTRet.init();
  int count = 0;

  for (int j = 0; j < 30; ++j)
  {
    _QUEST_DB_BASE::_LIST *slot = &this->m_pQuestData->m_List[j];
    if (slot->wIndex != 0xFFFF)
    {
      _base_fld *record = CQuestMgr::s_tblQuest ? CQuestMgr::s_tblQuest->GetRecord(slot->wIndex) : nullptr;
      if (record)
      {
        if (record[1].m_dwIndex != static_cast<unsigned int>(-1)
            && static_cast<int>(record[1].m_dwIndex) < nNewLv)
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

char CQuestMgr::CheckFailCondition(unsigned __int8 byQuestDBSlot, int nFailCond, char *pszCode)
{
  char result = 0;
  if (!m_pMaster)
  {
    return result;
  }

  void *src = &m_pMaster->m_Param.m_QuestDB.m_List[byQuestDBSlot];
  _QUEST_DB_BASE::_LIST slot{};
  memcpy_0(&slot, src, sizeof(slot));

  if (*reinterpret_cast<unsigned __int16 *>(static_cast<char *>(src) + 1) == 0xFFFF)
  {
    return result;
  }

  _base_fld *record =
    CQuestMgr::s_tblQuest
      ? CQuestMgr::s_tblQuest->GetRecord(
          *reinterpret_cast<unsigned __int16 *>(static_cast<char *>(src) + 1))
      : nullptr;
  if (!record)
  {
    return result;
  }

  if (*reinterpret_cast<int *>(static_cast<char *>(src) + 9) == -1)
  {
    return result;
  }

  if (!*reinterpret_cast<int *>(&record[27].m_strCode[28]))
  {
    return result;
  }

  for (int j = 0; j < 3; ++j)
  {
    if (*reinterpret_cast<int *>(&record[j + 27].m_strCode[32]) == nFailCond)
    {
      if (nFailCond)
      {
        if (nFailCond == 1)
        {
          const int limitMin = atoi(&record[j + 27].m_strCode[36]);
          if (*reinterpret_cast<unsigned int *>(static_cast<char *>(src) + 9) < static_cast<unsigned int>(60 * limitMin))
          {
            result = 1;
          }
        }
        else if (!pszCode || !strcmp_0(pszCode, &record[j + 27].m_strCode[36]))
        {
          result = 1;
        }
      }
      else
      {
        const int limitMin = atoi(&record[j + 27].m_strCode[36]);
        if (*reinterpret_cast<unsigned int *>(static_cast<char *>(src) + 9) > static_cast<unsigned int>(60 * limitMin))
        {
          result = 1;
        }
      }

      if (result)
      {
        m_pMaster->SendMsg_QuestFailure(nFailCond, byQuestDBSlot);
        DeleteQuestData(byQuestDBSlot);
        m_pMaster->m_pUserDB->Update_QuestDelete(byQuestDBSlot);

        if (!*reinterpret_cast<int *>(&record[31].m_strCode[28]))
        {
          if (strcmp_0(&record[31].m_strCode[32], "-1"))
          {
            _dummy_position *pos = m_pMaster->m_pCurMap->GetDummyPostion(&record[31].m_strCode[32]);
            if (pos)
            {
              float newPos[8]{};
              if (m_pMaster->m_pCurMap->GetRandPosInDummy(pos, newPos, 1))
              {
                m_pMaster->OutOfMap(m_pMaster->m_pCurMap, m_pMaster->m_wMapLayerIndex, 3u, newPos);
                const unsigned __int8 mapCode = m_pMaster->m_Param.GetMapCode();
                m_pMaster->SendMsg_GotoRecallResult(0, mapCode, newPos, 4u);
              }
            }
          }
        }

        if (!strcmp_0(&record[32].m_strCode[28], "-1"))
        {
          return result;
        }

        _base_fld *nextRecord = CQuestMgr::s_tblQuest
          ? CQuestMgr::s_tblQuest->GetRecord(&record[32].m_strCode[28])
          : nullptr;
        if (!nextRecord)
        {
          return result;
        }

        if (slot.byQuestType != 1
            || (!IsCompleteNpcQuest(nextRecord->m_strCode, *reinterpret_cast<int *>(&nextRecord[1].m_strCode[4]))
                && !IsProcNpcQuest(nextRecord->m_strCode)))
        {
          *reinterpret_cast<unsigned __int8 *>(src) = slot.byQuestType;
          *reinterpret_cast<unsigned __int16 *>(static_cast<char *>(src) + 1) = nextRecord->m_dwIndex;
          *reinterpret_cast<unsigned int *>(static_cast<char *>(src) + 9) = 0;

          for (int k = 0; k < 3; ++k)
          {
            *reinterpret_cast<unsigned __int16 *>(static_cast<char *>(src) + 2 * k + 3) = 0xFFFF;
            if (*reinterpret_cast<int *>(&nextRecord[4 * k + 1].m_strCode[32]) != -1)
            {
              *reinterpret_cast<unsigned __int16 *>(static_cast<char *>(src) + 2 * k + 3) = 0;
            }
            if (*reinterpret_cast<int *>(&nextRecord[4 * k + 4].m_strCode[24]) == -1)
            {
              *reinterpret_cast<unsigned __int16 *>(static_cast<char *>(src) + 2 * k + 3) = 0xFFFF;
            }
          }

          m_pMaster->m_pUserDB->Update_QuestInsert(byQuestDBSlot, reinterpret_cast<_QUEST_DB_BASE::_LIST *>(src));
          m_pMaster->SendMsg_InsertNextQuest(byQuestDBSlot, reinterpret_cast<_QUEST_DB_BASE::_LIST *>(src));
          return result;
        }
      }
    }
  }

  return result;
}

_happen_event_cont *CQuestMgr::CheckQuestHappenEvent(
  QUEST_HAPPEN HappenType,
  char *pszEventCode,
  unsigned __int8 byRaceCode)
{
  _base_fld *record = CQuestMgr::s_tblQuestHappenEvent
    ? CQuestMgr::s_tblQuestHappenEvent[HappenType].GetRecord(pszEventCode)
    : nullptr;
  while (record)
  {
    auto *eventNode =
      reinterpret_cast<_happen_event_node *>(&record[1].m_strCode[704 * static_cast<int>(byRaceCode)]);
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
        _base_fld *questRecord =
          CQuestMgr::s_tblQuest ? CQuestMgr::s_tblQuest->GetRecord(eventNode->m_strLinkQuest[0]) : nullptr;
        if (!questRecord)
        {
          return nullptr;
        }
        if (questRecord[1].m_dwIndex != -1)
        {
          const int level = static_cast<int>(this->m_pMaster->m_Param.GetLevel());
          if (static_cast<int>(questRecord[1].m_dwIndex) < level)
          {
            return nullptr;
          }
        }
        if (!IsCompleteNpcQuest(questRecord->m_strCode, *reinterpret_cast<int *>(&questRecord[1].m_strCode[4]))
            && !IsProcNpcQuest(questRecord->m_strCode))
        {
          if (*reinterpret_cast<int *>(&questRecord[1].m_strCode[4]) == 1)
          {
            if (!IsProcLinkNpcQuest(questRecord->m_strCode, *reinterpret_cast<int *>(&questRecord[27].m_strCode[24])))
            {
              return nullptr;
            }
            if (!IsPossibleRepeatNpcQuest(questRecord->m_strCode, *reinterpret_cast<int *>(&questRecord[27].m_strCode[24])))
            {
              return nullptr;
            }
          }
        }
        else
        {
          record = CQuestMgr::s_tblQuestHappenEvent
            ? CQuestMgr::s_tblQuestHappenEvent[HappenType].GetRecord(record->m_dwIndex + 1)
            : nullptr;
          if (record && strcmp_0(record->m_strCode, pszEventCode))
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
      ? CQuestMgr::s_tblQuestHappenEvent[HappenType].GetRecord(record->m_dwIndex + 1)
      : nullptr;
    if (record && strcmp_0(record->m_strCode, pszEventCode))
    {
      record = nullptr;
    }
  }

  return nullptr;
}

_Quest_fld *CQuestMgr::GetQuestFromEvent(unsigned __int8 bySelect)
{
  if (!this->m_LastHappenEvent.isset())
  {
    return nullptr;
  }

  char *recordCode = nullptr;
  int linkCount = 0;
  for (int j = 0; j < 5 && strncmp(this->m_LastHappenEvent.m_pEvent->m_strLinkQuest[j], "-1", 2u); ++j)
  {
    ++linkCount;
  }

  if (linkCount > 0)
  {
    if (this->m_LastHappenEvent.m_pEvent->m_bSelectQuestManual)
    {
      if (bySelect >= linkCount)
      {
        return nullptr;
      }
      recordCode = this->m_LastHappenEvent.m_pEvent->m_strLinkQuest[bySelect];
    }
    else
    {
      recordCode = this->m_LastHappenEvent.m_pEvent->m_strLinkQuest[rand() % linkCount];
    }
  }

  this->m_LastHappenEvent.init();
  if (recordCode)
  {
    return CQuestMgr::s_tblQuest ? reinterpret_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(recordCode))
                                 : nullptr;
  }
  return nullptr;
}

__int64 CQuestMgr::GetCountQuestType(int nType)
{
  unsigned int count = 0;
  for (int j = 0; j < 30; ++j)
  {
    _QUEST_DB_BASE::_LIST *slot = &this->m_pQuestData->m_List[j];
    if (slot->byQuestType != 255 && slot->byQuestType == nType)
    {
      ++count;
    }
  }
  return count;
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

char CQuestMgr::__CheckCond_LV(unsigned __int8 byCompare, int nLv)
{
  if (byCompare)
  {
    if (byCompare == 1)
    {
    if (this->m_pMaster->m_Param.GetLevel() < static_cast<unsigned int>(nLv))
      {
        return 1;
      }
    }
    else if (byCompare == 2 && this->m_pMaster->m_Param.GetLevel() > static_cast<unsigned int>(nLv))
    {
      return 1;
    }
  }
  else if (this->m_pMaster->m_Param.GetLevel() == static_cast<unsigned int>(nLv))
  {
    return 1;
  }
  return 0;
}

char CQuestMgr::__CheckCond_Grade(unsigned __int8 byCompare, int nGrade)
{
  if (byCompare)
  {
    if (byCompare == 1)
    {
      if (this->m_pMaster->m_Param.m_byPvPGrade < nGrade)
      {
        return 1;
      }
    }
    else if (byCompare == 2 && this->m_pMaster->m_Param.m_byPvPGrade > nGrade)
    {
      return 1;
    }
  }
  else if (this->m_pMaster->m_Param.m_byPvPGrade == nGrade)
  {
    return 1;
  }
  return 0;
}

char CQuestMgr::__CheckCond_Dalant(unsigned __int8 byCompare, unsigned int nDalant)
{
  if (byCompare)
  {
    if (byCompare == 1)
    {
      if (this->m_pMaster->m_Param.GetDalant() < nDalant)
      {
        return 1;
      }
    }
    else if (byCompare == 2 && this->m_pMaster->m_Param.GetDalant() > nDalant)
    {
      return 1;
    }
  }
  else if (this->m_pMaster->m_Param.GetDalant() == nDalant)
  {
    return 1;
  }
  return 0;
}

char CQuestMgr::__CheckCond_Gold(unsigned __int8 byCompare, unsigned int nGold)
{
  if (byCompare)
  {
    if (byCompare == 1)
    {
      if (this->m_pMaster->m_Param.GetGold() < nGold)
      {
        return 1;
      }
    }
    else if (byCompare == 2 && this->m_pMaster->m_Param.GetGold() > nGold)
    {
      return 1;
    }
  }
  else if (this->m_pMaster->m_Param.GetGold() == nGold)
  {
    return 1;
  }
  return 0;
}

bool CQuestMgr::__CheckCond_Party(int belong)
{
  if (belong)
  {
    return this->m_pMaster->m_pPartyMgr && !this->m_pMaster->m_pPartyMgr->IsPartyMode();
  }
  return this->m_pMaster->m_pPartyMgr && this->m_pMaster->m_pPartyMgr->IsPartyMode();
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
  (void)belong;
  return false; // this is not a stub
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

bool CQuestMgr::__CheckCond_Mastery(unsigned __int8 byCompare, char *pszMasteryPack)
{
  (void)byCompare;
  char codeBuf[32]{};
  char indexBuf[32]{};
  char levelBuf[16]{};

  memcpy_0(codeBuf, pszMasteryPack, 1u);
  codeBuf[1] = 0;
  memcpy_0(indexBuf, pszMasteryPack + 1, 2u);
  indexBuf[2] = 0;
  memcpy_0(levelBuf, pszMasteryPack + 3, 2u);
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

char CQuestMgr::__CheckCond_Dummy(int bInclude, char *pszDummyCode)
{
  (void)bInclude;
  (void)pszDummyCode;
  return 1; // this is not a stub
}

bool CQuestMgr::__CheckCond_Race(char *pszRaceSexFlag)
{
  const int raceSex = static_cast<int>(this->m_pMaster->m_Param.GetRaceSexCode());
  return pszRaceSexFlag[raceSex] == '1';
}

char CQuestMgr::__CheckCond_Class(char *pszClassCode)
{
  for (int j = 0; j < 4; ++j)
  {
    const __int64 effectPtr = reinterpret_cast<__int64>(*this->m_pMaster->m_Param.m_ppHistoryEffect[j]);
    if (!effectPtr)
    {
      break;
    }
    if (!strcmp_0(reinterpret_cast<const char *>(effectPtr + 4), pszClassCode))
    {
      return 1;
    }
  }
  return 0;
}

void CQuestMgr::DeleteQuestData(unsigned __int8 bySlot)
{
  _QUEST_DB_BASE::_LIST *slot = &this->m_pQuestData->m_List[bySlot];
  if (slot->byQuestType != 255)
  {
    slot->Init();
  }
}

char CQuestMgr::IsCompleteNpcQuest(char *pszCode, int bQuestRepeat)
{
  for (int j = 0; j < 70; ++j)
  {
    _QUEST_DB_BASE::_NPC_QUEST_HISTORY &history = this->m_pQuestData->m_History[j];
    if (history.byLevel != 255 && !strncmp(history.szQuestCode, pszCode, 7u) && bQuestRepeat != 1)
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
    _QUEST_DB_BASE::_LIST *slot = &this->m_pQuestData->m_List[j];
    if (slot->byQuestType == 1)
    {
      _base_fld *record = CQuestMgr::s_tblQuest ? CQuestMgr::s_tblQuest->GetRecord(slot->wIndex) : nullptr;
      if (!strncmp(record->m_strCode, pszCode, 7u))
      {
        return 1;
      }
    }
  }
  return 0;
}

char CQuestMgr::IsProcLinkNpcQuest(char *pszCode, int nLinkQuestGroupID)
{
  (void)pszCode;
  for (int j = 0; j < 30; ++j)
  {
    _QUEST_DB_BASE::_LIST *slot = &this->m_pQuestData->m_List[j];
    if (slot->byQuestType == 1)
    {
      _base_fld *record = CQuestMgr::s_tblQuest ? CQuestMgr::s_tblQuest->GetRecord(slot->wIndex) : nullptr;
      if (record)
      {
        if (*reinterpret_cast<int *>(&record[27].m_strCode[24]) == nLinkQuestGroupID)
        {
          return 0;
        }
      }
    }
  }
  return 1;
}

bool CQuestMgr::IsPossibleRepeatNpcQuest(char *pszCode, int nLinkQuestGroupID)
{
  (void)pszCode;
  int repeatCount = 0;
  for (int j = 0; j < 70; ++j)
  {
    _QUEST_DB_BASE::_NPC_QUEST_HISTORY &history = this->m_pQuestData->m_History[j];
    if (history.byLevel != 255)
    {
      _base_fld *record =
        CQuestMgr::s_tblQuest ? CQuestMgr::s_tblQuest->GetRecord(history.szQuestCode) : nullptr;
      if (record && *reinterpret_cast<int *>(&record[27].m_strCode[24]) == nLinkQuestGroupID)
      {
        const unsigned int now = GetConnectTime_AddBySec(0);
        if (history.dwEventEndTime >= now)
        {
          ++repeatCount;
        }
      }
    }
  }
  return repeatCount == 0;
}
