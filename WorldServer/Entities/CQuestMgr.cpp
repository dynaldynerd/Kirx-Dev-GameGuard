#include "pch.h"

#include "CQuestMgr.h"

#include "CRecordData.h"

CRecordData *CQuestMgr::s_tblQuest = nullptr;
CRecordData *CQuestMgr::s_tblQuestHappenEvent = nullptr;

bool CQuestMgr::LoadQuestData()
{
  static CRecordData s_tblQuestStorage;
  static CRecordData s_tblQuestHappenEventStorage[9];

  if (s_tblQuestHappenEvent == nullptr)
  {
    s_tblQuestHappenEvent = s_tblQuestHappenEventStorage;
  }
  if (s_tblQuest == nullptr)
  {
    s_tblQuest = &s_tblQuestStorage;
  }

  return true;
}
