#include "pch.h"

#include "CQuestMgr.h"

#include "CRecordData.h"
#include "QuestHappenEvent_fld.h"

#include <cstring>

CRecordData *CQuestMgr::s_tblQuest = nullptr;
CRecordData *CQuestMgr::s_tblQuestHappenEvent = nullptr;

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
